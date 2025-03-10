#ifndef messenger_h
#define messenger_h

#include <SSLClient.h>
#include <UrlEncode.h>

#define MESSENGER_SERVER "api.callmebot.com"

// +international_country_code + phone number
// Portugal +351, example: +351912345678
// TODO:
String phoneNumber = "17326198507"; //"REPLACE_WITH_YOUR_PHONE_NUMBER";
String apiKey = "6749268"; // "REPLACE_WITH_API_KEY";

void messenger_send(Unit_PoESP32 *eth, String phoneNumber, String apiKey, String message) {
  auto resp = eth->createSSLClient(MESSENGER_SERVER, 443);
  int idx = resp.indexOf("CONNECT");
  Serial.printf("idx %d\n", idx);
  if (idx != -1 && idx > 1 && idx < 1024) {
    int connectionId = resp.charAt(idx - 2) - '0';  // Get the connection ID
    Serial.print("Established connection ID: ");
    Serial.println(connectionId);

    String req = String("GET ") + "/whatsapp.php?phone=" + phoneNumber
                 + "&apikey=" + apiKey + "&text=" + urlEncode(message) + " HTTP/1.1\nHost: " + MESSENGER_SERVER 
                 + "\nConnection: close\n\n";
    eth->sendCMD("AT+CIPSEND=" + String(connectionId) + "," + String(req.length()));
    delay(100);
    eth->sendCMD(req);
    delay(2000);
  }
}

#endif
