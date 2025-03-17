/*
   @Hardware: M5AtomS3 + M5Stack ESP32 Ethernet Unit with PoE

   https://docs.m5stack.com/en/core/AtomS3%20Lite

   https://docs.m5stack.com/en/unit/poesp32
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

#define ALARM_PERIOD_SEC (15 * 60)
#define STATUS_PERIOD_SEC (12 * 60 * 60)

#define VOLTAGE_ALARM_THRESHOLD  11.7
#define LEVEL_CM_ALARM_THRESHOLD 25.0

#define RUN_TIME_MS 120000

bool ethUp = false;
bool ntpUp = false;
bool webServerUp = false;
bool send_alarms = false;

unsigned long start_time = 0UL;

void reportIpAddress() {
  auto IP = eth.getLocalIP();
  if (IP.length() > 0) {
    gen_nmea0183_msg("$BBTXT,01,01,01,LocalIP: %s", IP.c_str());
  }
}

void set_time(struct tm *timeinfo) {
  time_t t = mktime(timeinfo);
  struct timeval tv = {
    .tv_sec = t,
    .tv_usec = 0
  };
  settimeofday(&tv, NULL);
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

  if (phoneNumber.length() > 4 && apiKey.length() > 4) {
    send_alarms = true;
  }

  eth.initETH(&Serial2, 9600, G1, G2);

  mcu_sensors_scan();

  gen_nmea0183_txt("Waiting for ethernet device connected");
  while (!eth.checkDeviceConnect()) {
    delay(10);
  }

  gen_nmea0183_txt("Resetting device");
  eth.sendCMD("AT+RST");
  eth.waitMsg(2000, "OK");

  gen_nmea0183_txt("Waiting for ethernet device connected");
  while (!eth.checkDeviceConnect()) {
    delay(10);
  }
  
  //eth.sendCMD("AT+UART_CUR=115200,8,1,0,0"); //
  //eth.waitMsg(1000, "OK", "ERROR");

  app.onRepeat(1000, []() {
    if (!ethUp) {
      gen_nmea0183_txt("Waiting for ethernet connected");
      ethUp = eth.checkETHConnect();
      if (ethUp) {
        auto localInfo = eth.obtainLocalIP();
        if (localInfo.length() > 0) {
          reportIpAddress();
        }
        
        eth.sendCMD("AT+CIPMODE=0");
        eth.waitMsg(100, "OK");

        auto muxResponse = eth.activateMUXMode();

        auto ntpRes = eth.activateNTPClient();
        if (ntpRes.indexOf("OK") != -1) {
          struct tm timeinfo;
          ntpUp = eth.getNTPTime(&timeinfo);
          if (ntpUp) {
            set_time(&timeinfo);
            gen_nmea0183_txt("Got time from NTP");    
          }
        }
      }
    }
  });

  if (!send_alarms) {
    app.onRepeat(1000, []() {
      if (ethUp && !webServerUp) {
        gen_nmea0183_txt("Waiting for web server start");

        eth.sendCMD("AT+CIPSERVER=0,1"); // shutdown server and close connections
        eth.waitMsg(100, "OK", "ERROR");

        auto servActivationResponse = eth.activateTcpServerPort80();
        webServerUp = servActivationResponse.indexOf("OK") != -1;
      }
    });
  }

  app.onRepeat(30000, []() {
    if (ethUp) {
      reportIpAddress();
    }
  });

  start_time = millis();
}

void loop() {
  M5.update();
  app.tick();

  // Check for incoming data from the Ethernet unit
  if (webServerUp && Serial2.available() > 4) {
    String response = eth.waitMsg(2000, "Host:");

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

      if (httpRequest.startsWith("GET") && connectionId >= 0) {
        HttpRequest parsedRequest = parseHttpRequest(httpRequest);
        //log_http_request(parsedRequest);

        if (parsedRequest.path.equals("/settings")) {
          for (int i = 0; i < parsedRequest.queryArgCount; i++) {
            if (parsedRequest.queryArgs[i].key.equals("phone")) {
              phoneNumber = parsedRequest.queryArgs[i].value;
            } else if (parsedRequest.queryArgs[i].key.equals("key")) {
              apiKey = parsedRequest.queryArgs[i].value;
            }
          }
          save_settings(phoneNumber, apiKey);
          gen_nmea0183_msg("$BBTXT,01,01,01,Stored settings. %s",
                           (String("phone:") + phoneNumber + String(" apiKey:") + apiKey).c_str());          
          handle_OnSettings(&eth, connectionId);
          delay(200);
        } else if (parsedRequest.path.equals("/")) {
          handle_OnConnect(&eth, connectionId);
          delay(200);
        } else {
          handle_NotFound(&eth, connectionId, parsedRequest.path);
          delay(150);
        }
        eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
        eth.waitMsg(500, "OK", "ERROR");
      }
    }
  }
  if (ethUp && M5.BtnA.wasPressed()) {
    String message = "Hello from esp32!";
    if (phoneNumber.length() > 4 && apiKey.length() > 4) {
      messenger_send(&eth, phoneNumber, apiKey, message);
    }
  }
  if (ethUp && ntpUp && send_alarms) {
    float voltage = i2c_ads1115_voltage(&i2c_ads1115_sensor_1);
    float water_dist_to_sensor = gpio_jsn_sr04t_distance_cm();
    bool raise_voltage_alarm = fabs(voltage) > 0.0001 && fabs(voltage) < VOLTAGE_ALARM_THRESHOLD;
    bool raise_bilge_alarm = fabs(water_dist_to_sensor) < LEVEL_CM_ALARM_THRESHOLD;

    uint64_t epoch_now = (uint64_t) time(NULL);
    if (raise_voltage_alarm || raise_bilge_alarm) {
      uint64_t last_alarm = get_last_alarm_time();
      if (last_alarm == 0 || epoch_now - last_alarm > ALARM_PERIOD_SEC) {
        String message = "Alarm";
        message += (raise_voltage_alarm ? String(" Low Voltage: ") : String(" Voltage: ")) + String(voltage);
        message += (raise_bilge_alarm ? String(" High Bilge: ") : String(" Bilge: ")) + String(water_dist_to_sensor);
        gen_nmea0183_msg("$BBTXT,01,01,01,%s", String(message).c_str());
        if (messenger_send(&eth, phoneNumber, apiKey, message)) {
          save_last_alarm_time(epoch_now);
        }
      }
    }
    uint64_t last_heartbeat = get_last_heartbeat_time();
    if (last_heartbeat == 0 || epoch_now - last_heartbeat > STATUS_PERIOD_SEC) {
      String message = "Status Voltage: " + String(voltage) + " Bilge: " + String(water_dist_to_sensor);
      gen_nmea0183_msg("$BBTXT,01,01,01,%s", String(message).c_str());
      if (messenger_send(&eth, phoneNumber, apiKey, message)) {
        save_last_heartbeat_time(epoch_now);
      }  
    }
  }
  if (send_alarms && (millis() - start_time > RUN_TIME_MS)) {
    gen_nmea0183_msg("$BBTXT,01,01,01,%s", "Going to sleep mode");
    eth.sendCMD("AT+GSLP=" + String(SLEEP_DURATION / 1000 - 5000));
    eth.waitMsg(100, "OK", "ERROR");
    delay(200);
    cat_nap(SLEEP_DURATION);
  }
}
