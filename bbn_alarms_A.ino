/**
   @Hardware: M5AtomS3 + M5Stack ESP32 Ethernet Unit with PoE
*/

#include <M5Unified.h> 
#include <SPI.h>
#include <M5_Ethernet.h>
#include <ReactESP.h>  // https://github.com/mairas/ReactESP

using namespace reactesp;
ReactESP app;

#include "conf_store.h"
#include "sleep_n_wakeup.h"
#include "messenger.h"
#include "web_server.h"
#include "i2c_ads1115.h"
#include "gpio_jsn_sr04t.h"

// M5AtomLiteS3
#define SCK  5
#define MISO 7
#define MOSI 8
#define CS   6

// M5AtomLite
//#define SCK  22
//#define MISO 23
//#define MOSI 33
//#define CS   19

byte mac[] = {0xDE, 0xAD, 0xBE, 0xDE, 0xCA, 0xDE};  // Host name will be WIZnetDECADE on local LAN (WIZnet + 3 last MAC octets)

EthernetServer server(80);

// Choose the analog pin to get semi-random data from for SSL
// Pick a pin that's not connected or attached to a randomish voltage source
const int rand_pin = 2; // 34 /* atom-lite */;

// Initialize the SSL client library
// We input an EthernetClient, our trust anchors, and the analog pin
EthernetClient base_client;
SSLClient ssl_client(base_client, TAs, (size_t)TAs_NUM, rand_pin);

unsigned long byteCount = 0;

void setup() {
  M5.begin();
  Serial.begin(115200);

  Serial.println("Initializing SPI...");
  SPI.begin(SCK, MISO, MOSI, -1);
  Serial.println("Initializing ethernet...");
  Ethernet.init(CS);

  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(2000);
  }

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println(
      "Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);  // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");

  // start the server
  server.begin();
  Serial.print("Server is at http://");
  Serial.println(Ethernet.localIP());
}


void loop() {
  M5.update();
  app.tick();

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
        if (c == ' ' /* after GET or POST */ || c == '?') argument_reading++;
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

  if (M5.BtnA.wasPressed()) {
    Serial.println("BtnA.wasPressed");
    if (ssl_client.connect(SERVER, 443)) {
      Serial.print("connected to ");
      // Make a HTTP request:
      ssl_client.println(String("GET ") + "/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message) + " HTTP/1.1");
      ssl_client.println("Host: api.callmebot.com");
      ssl_client.println("Connection: close");
      ssl_client.println();
    } else {
      // if you didn't get a connection to the server:
      Serial.println("connection failed");
    }
  }

  if (ssl_client.connected()) {
    // if there are incoming bytes available
    // from the server, read them and print them:
    int len;
    while (len = ssl_client.available()) {
      if (len > 0) {
        byte buffer[80];
        if (len > 80) len = 80;
        ssl_client.read(buffer, len);
        byteCount = byteCount + len;
      }
      delay(1);
    }
  }

  // server's disconnected
  if (!ssl_client.connected() && byteCount > 0) {
    Serial.println("disconnecting.");
    //ssl_client.stop();
    Serial.print("Received ");
    Serial.print(byteCount);
    byteCount = 0;
  }
}
