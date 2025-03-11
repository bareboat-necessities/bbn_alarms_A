#ifndef web_server_h
#define web_server_h

#include "Unit_PoESP32_ext.h"

#define http_headers "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"

#define conf_stored R"=====(
<div><p>Settings stored.</p></div>)====="

#define form_box R"=====(
<form id="form" method="GET" action="/settings">
<p>Enter your phone number and generated CallMeBot API key for WhatsApp. For details, see:
 <a target="_blank" rel="noopener noreferrer" href="https://www.callmebot.com/blog/free-api-whatsapp-messages/">this guide</a>.
</p>
<label for="phone">Phone Number:</label>
<input id="phone" type="text" name="phone" value="" placeholder="+PhoneNumber" required>
<label for="api_key">API Key:</label>
<input id="api_key" type="text" name="api_key" value="" placeholder="API Key" required>
<input id="button" type="submit" value="Submit">
</form>)====="

#define settings_page_head R"=====(
<!DOCTYPE html>
<html lang="en">
<head>
 <meta charset="UTF-8">
 <meta name="viewport" content="width=device-width,initial-scale=1.0">
 <title>WhatsApp API Config</title>
 <style>
body {
 font-family: Arial, sans-serif;
 background-color: #121212;
 color: #e0e0e0;
 margin: 0;
 padding: 0;
 display: flex;
 justify-content: center;
 align-items: center;
 height: 100vh;
}
.fbox {
 background-color: #1e1e1e;
 padding: 2rem;
 border-radius: 8px;
 box-shadow: 0 4px 6px rgba(0, 0, 0, 0.3);
 max-width: 400px;
 width: 100%;
}
.fbox h1 {
 font-size: 1.5rem;
 margin-bottom: 1rem;
 color: #ffffff;
}
.fbox label {
 font-weight: bold;
 color: #bbbbbb;
}
.fbox input[type="text"] {
 width: 100%;
 padding: 0.75rem;
 margin: 0.5rem 0 1rem 0;
 border: 1px solid #444;
 border-radius: 4px;
 font-size: 1rem;
 background-color: #2d2d2d;
 color: #e0e0e0;
}
.fbox input[type="text"]::placeholder {
 color: #888;
}
.fbox input[type="submit"] {
 background-color: #007bff;
 color: #fff;
 border: none;
 padding: 0.75rem 1.5rem;
 border-radius: 4px;
 font-size: 1rem;
 cursor: pointer;
 transition: background-color 0.3s ease;
}
.fbox input[type="submit"]:hover {
 background-color: #0056b3;
}
.fbox a {
 color: #007bff;
 text-decoration: none;
}
.fbox a:hover {
 text-decoration: underline;
}
 </style>
</head>
<body>
<div class="fbox">
 <h1>WhatsApp API Configuration</h1>)====="

#define settings_page_tail R"=====(
</div>
</body>
</html>)====="

const char config_page[] PROGMEM = "" http_headers settings_page_head "" 
 form_box settings_page_tail;

const char confirmation_page[] PROGMEM = "" http_headers settings_page_head "" 
 conf_stored settings_page_tail;

String build_begin_response(Unit_PoESP32 *client, int connectionId, int request_status = 200) {
  return (String("HTTP/1.1 ") + String(request_status) + (request_status == 200 ? String(" OK\r\n") : String(" Not Found\r\n")) +
     "Content-Type: text/html\r\nConnection: close\r\n\r\n");
}

void begin_response(Unit_PoESP32 *client, int connectionId, int request_status = 200) {
  client->sendTCPString(connectionId, 
    build_begin_response(client, connectionId, request_status).c_str());
}

void main_page(Unit_PoESP32 *client, int connectionId, bool stored, int request_status = 200) {
  if (stored) {
    client->sendTCPString(connectionId, confirmation_page);
  } else {
    client->sendTCPString(connectionId, config_page);
  }
}

void error_page(Unit_PoESP32 *client, int connectionId, int request_status = 200) {
  begin_response(client, connectionId, request_status);
}

void handle_OnConnect(Unit_PoESP32 *client, int connectionId) {
  main_page(client, connectionId, false);
}

void handle_OnSettings(Unit_PoESP32 *client, int connectionId) {
  main_page(client, connectionId, true);
}

void handle_NotFound(Unit_PoESP32 *client, int connectionId, String url) {
  error_page(client, connectionId, 400);
  client->sendTCPString(connectionId, String("<a>The path " + url + " doesn't exist</a>\n").c_str());
}

#endif
