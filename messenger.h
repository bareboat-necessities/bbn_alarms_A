#ifndef messenger_h
#define messenger_h

#include <SSLClient.h>
#include <UrlEncode.h>

#include "trust_anchors.h"

#define SERVER "api.callmebot.com"

// +international_country_code + phone number
// Portugal +351, example: +351912345678
// TODO:
String phoneNumber = "REPLACE_WITH_YOUR_PHONE_NUMBER";
String apiKey = "REPLACE_WITH_API_KEY";
String message = "Test from M5 ESP32";


#endif
