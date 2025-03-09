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

Unit_PoESP32 eth;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);

  Serial.println("device connecting");
  eth.Init(&Serial2, 9600, G1, G2);
  Serial.println("wait device connect");
  while (!eth.checkDeviceConnect()) {
    delay(10);
  }
  Serial.println("device connected");

  Serial.println("wait ethernet connect");
  while (!eth.checkETHConnect()) {
    delay(10);
  }
  Serial.println("ethernet connected");

  auto localInfo = eth.obtainLocalIP();
  Serial.println(localInfo.c_str());

  eth.sendCMD("AT+CIPSERVER=0,1"); // shutdown server and close connections
  eth.waitMsg(100, "OK", "ERROR");
  
  eth.sendCMD("AT+CIPMODE=0");
  eth.waitMsg(100, "OK");
   
  auto muxResponse = eth.activateMUXMode();
  Serial.println(muxResponse.c_str());

  auto servActivationResponse = eth.activateTcpServerPort80();
  Serial.println(servActivationResponse.c_str());
}

void loop() {
  M5.update();
  app.tick();

  // Check for incoming data from the Ethernet unit
  if (Serial2.available() > 4) {
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

        // Print the parsed request
        Serial.println("Method: " + parsedRequest.method);
        Serial.println("Path: " + parsedRequest.path);
        Serial.println("Query String: " + parsedRequest.queryString);
        Serial.println("Query Arguments:");
        for (int i = 0; i < parsedRequest.queryArgCount; i++) {
          Serial.println("  " + parsedRequest.queryArgs[i].key + " = " + parsedRequest.queryArgs[i].value);
        }
        Serial.println("Headers:");
        Serial.println(parsedRequest.headers);

        handle_OnConnect(&eth, connectionId);
        delay(500);
        
        eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
        delay(1000);
      }
    }
  }
  if (M5.BtnA.wasPressed()) {
    Serial.println("BtnA.wasPressed");
    auto resp = eth.createSSLClient(MESSENGER_SERVER, 443);
    int idx = resp.indexOf("CONNECT");
    Serial.printf("idx %d\n", idx);
    if (idx != -1 && idx > 1 && idx < 1024) {
      int connectionId = resp.charAt(idx - 2) - '0';  // Get the connection ID
      Serial.print("Established connection ID: ");
      Serial.println(connectionId);

      String message = "Hello from esp32!";
      String req = String("GET ") + "/whatsapp.php?phone=" + phoneNumber
                   + "&apikey=" + apiKey + "&text=" + urlEncode(message) + " HTTP/1.1\nHost: api.callmebot.com\nConnection: close\n\n";
      eth.sendCMD("AT+CIPSEND=" + String(connectionId) + "," + String(req.length()));
      delay(100);
      eth.sendCMD(req);
      delay(2000);
    }
  }
}
