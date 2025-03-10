/**
   @Hardware: M5AtomS3 + M5Stack ESP32 Ethernet Unit with PoE
*/

#include <M5Unified.h>
#include <SPI.h>
#include <ReactESP.h>  // https://github.com/mairas/ReactESP

using namespace reactesp;
ReactESP app;

#include "Unit_PoESP32_ext.h"
#include "messenger.h"
#include "conf_store.h"
#include "sleep_n_wakeup.h"
#include "http_parsing.h"
#include "web_server.h"
#include "i2c_ads1115.h"
#include "gpio_jsn_sr04t.h"

static const char* firmware_tag = "bbn_alarms_A";

Unit_PoESP32 eth;

bool ethUp = false;
bool webServerUp = false;

void reportIpAddress() {
  auto IP = eth.getLocalIP();
  if (IP.length() > 0) {
    gen_nmea0183_msg("$BBTXT,01,01,01,LocalIP: %s", IP.c_str());
  }
}

void mcu_sensors_scan() {
  i2c_ads1115_try_init(&Wire1, G38, G39, 100000UL);
  gpio_jsn_sr04t_try_init();
}

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);

  gen_nmea0183_msg("$BBTXT,01,01,01,FirmwareTag: %s", firmware_tag);

  restore_settings();
  gen_nmea0183_msg("$BBTXT,01,01,01,Loaded settings. %s",
                   (String("phone:") + phoneNumber + String(" apiKey:") + apiKey).c_str());

  eth.initETH(&Serial2, 9600, G1, G2);

  mcu_sensors_scan();

  gen_nmea0183_txt("Waiting for ethernet device connected");
  while (!eth.checkDeviceConnect()) {
    delay(10);
  }

  app.onRepeat(1000, []() {
    if (!ethUp) {
      gen_nmea0183_txt("Waiting for ethernet connected");
      ethUp = eth.checkETHConnect();
      if (ethUp) {
        auto localInfo = eth.obtainLocalIP();
        if (localInfo.length() > 0) {
          reportIpAddress();
        }
      }
    }
  });

  app.onRepeat(1000, []() {
    if (ethUp && !webServerUp) {
      gen_nmea0183_txt("Waiting for web server start");

      eth.sendCMD("AT+CIPSERVER=0,1"); // shutdown server and close connections
      eth.waitMsg(100, "OK", "ERROR");

      eth.sendCMD("AT+CIPMODE=0");
      eth.waitMsg(100, "OK");

      auto muxResponse = eth.activateMUXMode();
      auto servActivationResponse = eth.activateTcpServerPort80();
      webServerUp = servActivationResponse.indexOf("OK") != -1;
    }
  });

  app.onRepeat(30000, []() {
    if (ethUp) {
      reportIpAddress();
    }
  });
}

void loop() {
  M5.update();
  app.tick();

  // Check for incoming data from the Ethernet unit
  if (webServerUp && Serial2.available() > 4) {
    String response = eth.waitMsg(200, "Connection:");

    // Check if a new client has connected
    int idx = response.indexOf("+IPD");
    if (idx != -1 && idx < 1024) {
      // Extract the connection ID and data
      int connectionId = response.charAt(5 + idx) - '0';  // Get the connection ID
      String httpRequest = response.substring(7 + idx);   // Get the data

      int idx2 = httpRequest.indexOf(":");
      if (idx2 != -1) {
        httpRequest = httpRequest.substring(idx2 + 1);
      }

      gen_nmea0183_msg("$BBTXT,01,01,01,Received data from connID: %s", String(connectionId).c_str());

      if (httpRequest.startsWith("GET")) {
        HttpRequest parsedRequest = parseHttpRequest(httpRequest);
        log_http_request(parsedRequest);

        if (parsedRequest.path.equals("/settings")) {
          for (int i = 0; i < parsedRequest.queryArgCount; i++) {
            if (parsedRequest.queryArgs[i].key.equals("phone")) {
              phoneNumber = parsedRequest.queryArgs[i].value;
            } else if (parsedRequest.queryArgs[i].key.equals("api_key")) {
              apiKey = parsedRequest.queryArgs[i].value;
            }
          }
          save_settings(phoneNumber, apiKey);
          gen_nmea0183_msg("$BBTXT,01,01,01,Stored settings. %s",
                           (String("phone:") + phoneNumber + String(" apiKey:") + apiKey).c_str());
          handle_OnSettings(&eth, connectionId);
          delay(2000);
        } else if (parsedRequest.path.equals("/")) {
          handle_OnConnect(&eth, connectionId);
          delay(2000);
        } else {
          handle_NotFound(&eth, connectionId, parsedRequest.path);
          delay(1000);
        }

        eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
        eth.waitMsg(1000, "OK", "ERROR");
      }
    }
  }
  if (ethUp && M5.BtnA.wasPressed()) {
    String message = "Hello from esp32!";
    if (phoneNumber.length() > 0 && apiKey.length() > 0) {
      messenger_send(&eth, phoneNumber, apiKey, message);
    }
  }
}
