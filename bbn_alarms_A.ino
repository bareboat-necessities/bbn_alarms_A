/**
   @Hardware: M5AtomS3 + M5Stack ESP32 Ethernet Unit with PoE
*/

#include <M5Unified.h>
#include <SPI.h>
#include <ReactESP.h>  // https://github.com/mairas/ReactESP

//using namespace reactesp;
//ReactESP app;

#include "Unit_PoESP32_ext.h"
#include "conf_store.h"
#include "sleep_n_wakeup.h"
#include "messenger.h"
#include "http_parsing.h"
#include "web_server.h"
//#include "i2c_ads1115.h"
//#include "gpio_jsn_sr04t.h"

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

  delay(100);
  auto muxResponse = eth.activateMUXMode();
  Serial.println(muxResponse.c_str());

  auto servActivationResponse = eth.activateTcpServerPort80();
  Serial.println(servActivationResponse.c_str());
}

void loop() {
  M5.update();

  // Check for incoming data from the Ethernet unit
  if (Serial2.available() > 4) {
    String response = eth.waitMsg(200, NULL, NULL);
    Serial.println(response);

    // Check if a new client has connected
    int idx = response.indexOf("+IPD");
    Serial.printf("idx %d\n", idx);
    if (idx != -1 && idx < 1024) {
      // Extract the connection ID and data
      int connectionId = response.charAt(5 + idx) - '0';  // Get the connection ID
      String httpRequest = response.substring(7 + idx);  // Get the data

      Serial.print("Received data from connection ID: ");
      Serial.println(connectionId);
      Serial.print("Data: ");
      Serial.println(httpRequest);

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
      delay(3000);

      eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
      delay(1000);

      // Send a response back to the client
      //String responseMessage = "Hello from M5Stack TCP Server!\n";
      //eth.sendCMD("AT+CIPSEND=" + String(connectionId) + "," + String(responseMessage.length()));
      //delay(100);
      //eth.sendCMD(responseMessage);
    }
  }
  if (M5.BtnA.wasPressed()) {
    Serial.println("BtnA.wasPressed");
    auto resp = eth.createSSLClient("api.callmebot.com", 443);
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
      delay(5000);
      eth.sendCMD("AT+CIPCLOSE=" + String(connectionId));
      delay(1000);
    }
  }
  //  app.tick();

  /*
    // listen for incoming clients
    EthernetClient client = server.available();
    if (client) {
      //Serial.println("new client");
      String url_path = "";
      String url_args = "";
      int argument_reading = 0;

      // An http request ends with a blank line
      boolean currentLineIsBlank = true;
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();

          // get arguments. note: path arguments must not have any blank spaces
          if (c == ' ' || c == '?') argument_reading++;
          if (argument_reading == 1 && c != ' ') url_path += c;
          if (argument_reading == 2 && c != '?') url_args += c;

          // check end of request
          if (c == '\n' && currentLineIsBlank) {
            // handle arguments
            Serial.println(url_path.c_str());
            Serial.println(url_args.c_str());
            if (url_path == "/") handle_OnConnect(client);
            else if (url_path == "/settings") handle_OnSettings(client);
            else handle_NotFound(client, url_path);
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
      // give the web browser time to receive the data
      delay(4);
      // close the connection:
      client.stop();
    }

  */
}
