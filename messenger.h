#ifndef messenger_h
#define messenger_h

#include <SSLClient.h>
#include <UrlEncode.h>

#include "Nmea0183Msg.h"

#define MESSENGER_SERVER "api.callmebot.com"

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "";
String apiKey = "";

bool messenger_send(Unit_PoESP32 *eth, String phoneNumber, String apiKey, String message) {
  auto resp = eth->createSSLClient(MESSENGER_SERVER, 443);
  int idx = resp.indexOf(",CONNECT");

  if (idx != -1 && idx > 0 && idx < 2048) {
    int connectionId = resp.charAt(idx - 1) - '0';  // Get the connection ID

    gen_nmea0183_msg("$BBTXT,01,01,01,Established connID: %s", String(connectionId).c_str());

    if (connectionId >= 0) {
      String req = String("GET ") + "/whatsapp.php?phone=" + phoneNumber
                   + "&apikey=" + apiKey + "&text=" + urlEncode(message) + " HTTP/1.1\r\nHost: " + MESSENGER_SERVER 
                   + "\r\nConnection: close\r\n\r\n";
      
      bool result = eth->sendTCPString(connectionId, req.c_str());
      delay(2000);
      return result;
    }
  }
  return false;
}

#endif
