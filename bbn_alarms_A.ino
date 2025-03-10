/**
   @Hardware: M5AtomS3 + M5Stack ESP32 Ethernet Unit with PoE
*/

#include <M5Unified.h>
#include <SPI.h>
#include <ReactESP.h>  // https://github.com/mairas/ReactESP

using namespace reactesp;
ReactESP app;

#include "Unit_PoESP32_ext.h"
#include "conf_store.h"
#include "sleep_n_wakeup.h"
#include "messenger.h"
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

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);

  gen_nmea0183_msg("$BBTXT,01,01,01,FirmwareTag: %s", firmware_tag);

  eth.initETH(&Serial2, 9600, G1, G2);

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
      Serial.println(muxResponse.c_str());

      auto servActivationResponse = eth.activateTcpServerPort80();
      Serial.println(servActivationResponse.c_str());

      webServerUp = true;
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
    Serial.println(response);

    // Check if a new client has connected
    int idx = response.indexOf("+IPD");
    Serial.printf("idx %d\n", idx);
    if (idx != -1 && idx < 1024) {
      // Extract the connection ID and data
      int connectionId = response.charAt(5 + idx) - '0';  // Get the connection ID
      String httpRequest = response.substring(7 + idx);  // Get the data

      int idx2 = httpRequest.indexOf(":");
      if (idx2 != -1) {
        httpRequest = httpRequest.substring(idx2 + 1);
      }

      Serial.print("Received data from connection ID: ");
      Serial.println(connectionId);
      Serial.print("Data: ");
      Serial.println(httpRequest);

      if (httpRequest.startsWith("GET")) {
        HttpRequest parsedRequest = parseHttpRequest(httpRequest);
        log_http_request(parsedRequest);

        handle_OnConnect(&eth, connectionId);
        delay(500);

        eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
        delay(1000);
      }
    }
  }
  if (ethUp && M5.BtnA.wasPressed()) {
    Serial.println("BtnA.wasPressed");
    String message = "Hello from esp32!";
    messenger_send(&eth, phoneNumber, apiKey, message);
  }
}
