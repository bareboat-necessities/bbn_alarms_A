#ifndef web_server_h
#define web_server_h

#include "Unit_PoESP32_ext.h"

#define http_headers "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"
#define http_err_headers "HTTP/1.1 400 Not Found\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"

#define conf_stored R"==1==(<div><p>Settings stored.</p></div>)==1=="

#define form_box R"==2==(<form id="form" method="GET" action="/settings">
<p>Enter your phone number and generated CallMeBot API key for WhatsApp. For details, see:
 <a target="_blank" rel="noopener noreferrer" href="https://www.callmebot.com/blog/free-api-whatsapp-messages/">this guide</a>.
</p>
<label for="phone">Phone:</label>
<input id="phone" type="text" name="phone" value="" placeholder="+PhoneNumber" required>
<label for="key">API Key:</label>
<input id="key" type="text" name="key" value="" placeholder="API Key" required>
<input id="button" type="submit" value="Submit">
</form>
)==2=="

#define settings_page_head R"==3==(<!DOCTYPE html>
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
 <h1>WhatsApp API Configuration</h1>)==3=="

#define settings_page_tail R"==4==(</div>
</body>
</html>
)==4=="

#define err_html R"==5==(<!DOCTYPE html>
<html lang="en">
<body>
<div><p>The requested path doesn't exist</p></div>
</body>
</html>
)==5=="

const char config_page[] PROGMEM = "" http_headers settings_page_head "" 
 form_box settings_page_tail;

const char confirmation_page[] PROGMEM = "" http_headers settings_page_head "" 
 conf_stored settings_page_tail;

const char err_page[] PROGMEM = "" http_err_headers err_html;

void main_page(Unit_PoESP32 *client, int connectionId, bool stored, int request_status = 200) {
  client->sendTCPString(connectionId, stored ? confirmation_page : config_page);
}

void handle_OnConnect(Unit_PoESP32 *client, int connectionId) {
  main_page(client, connectionId, false);
}

void handle_OnSettings(Unit_PoESP32 *client, int connectionId) {
  main_page(client, connectionId, true);
}

void handle_NotFound(Unit_PoESP32 *client, int connectionId, String url) {
  client->sendTCPString(connectionId, err_page);
}

#endif
