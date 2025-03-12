/*!
   Based on:

   @brief An an ESP32 Ethernet Unit supports PoE From M5Stack
   @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
   @copyright Copyright (c) 2025 by Bareboat Necessities

   @Links [Unit_PoESP32](https://docs.m5stack.com/en/unit/poesp32)
*/
#ifndef _UNIT_POESP32_EXT_H_
#define _UNIT_POESP32_EXT_H_

#include <Arduino.h>
#include "pins_arduino.h"

class Unit_PoESP32 {
  private:
    HardwareSerial *_serial;
    String _readstr;
    String _localIP = "";

  public:
    void initETH(HardwareSerial *serial = &Serial2, unsigned long baud = 9600,
                 uint8_t rx = G1, uint8_t tx = G2);
    String waitMsg(unsigned long time = 5, String expect_resp1 = "", String expect_resp2 = "");
    void sendCMD(String command);
    bool checkDeviceConnect();
    bool checkETHConnect();
    String obtainLocalIP();
    String getLocalIP();
    String activateMUXMode();
    String activateTcpServerPort80();
    String activateNTPClient();
    bool getNTPTime(struct tm *tm);
    String createTCPClient(String ip, int port);
    String createSSLClient(String ip, int port);
    bool sendTCPData(int connectionId, uint8_t *buffer, size_t size);
    bool sendTCPString(int connectionId, const char* string);
};

// Function to parse asctime-style string into struct tm
int parse_asctime(const char *asctime_str, struct tm *tm) {
  // Array of month abbreviations
  const char *months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };

  // Temporary variables to store parsed values
  char day[4], month[4];
  int tm_mday, tm_hour, tm_min, tm_sec, tm_year;

  // Parse the asctime string
  if (sscanf(asctime_str, "%3s %3s %d %d:%d:%d %d",
             day, month, &tm_mday, &tm_hour, &tm_min, &tm_sec, &tm_year) != 7) {
    return -1; // Parsing failed
  }

  // Convert month abbreviation to month number (0-11)
  tm->tm_mon = -1;
  for (int i = 0; i < 12; i++) {
    if (strcmp(month, months[i]) == 0) {
      tm->tm_mon = i;
      break;
    }
  }
  if (tm->tm_mon == -1) {
    return -1; // Invalid month
  }

  // Convert day abbreviation to day of the week (0-6, Sunday=0)
  tm->tm_wday = -1;
  const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  for (int i = 0; i < 7; i++) {
    if (strcmp(day, days[i]) == 0) {
      tm->tm_wday = i;
      break;
    }
  }
  if (tm->tm_wday == -1) {
    return -1; // Invalid day
  }

  // Assign parsed values to struct tm
  tm->tm_mday = tm_mday;
  tm->tm_hour = tm_hour;
  tm->tm_min = tm_min;
  tm->tm_sec = tm_sec;
  tm->tm_year = tm_year - 1900; // Years since 1900
  tm->tm_isdst = -1; // Daylight saving time information is not available

  return 0; // Success
}

/*! @brief Initialize the Unit PoESP32.*/
void Unit_PoESP32::initETH(HardwareSerial* serial, unsigned long baud, uint8_t rx, uint8_t tx) {
  _serial = serial;
  _serial->begin(baud, SERIAL_8N1, rx, tx);
}

/*! @brief Waiting for a period of time to receive a message
    @return Received messages.. */
String Unit_PoESP32::waitMsg(unsigned long time, String expect_resp1, String expect_resp2) {
  String restr = "";
  unsigned long start = millis();
  while (1) {
    if (_serial->available() > 0) {
      String str = _serial->readString();
      restr += str;
      if (expect_resp1.length() > 0 && restr.indexOf(expect_resp1) != -1) {
        break;
      }
      if (expect_resp2.length() > 0 && restr.indexOf(expect_resp2) != -1) {
        break;
      }
    }
    if ((millis() - start) > time) {
      break;
    }
  }
  return restr;
}

/*! @brief Send a command */
void Unit_PoESP32::sendCMD(String command) {
  _serial->println(command);
}

/*! @brief Detecting device connection status
    @return True if good connection, false otherwise. */
bool Unit_PoESP32::checkDeviceConnect() {
  sendCMD("AT");
  _readstr = waitMsg(1000, "OK", "ERROR");
  return _readstr.indexOf("OK") != -1;
}

/*! @brief Detecting device ETH connection status
    @return True if good connection, false otherwise. */
bool Unit_PoESP32::checkETHConnect() {
  sendCMD("AT+CIPETH?");
  _readstr = waitMsg(1000, "192", "ERROR");
  return _readstr.indexOf("192") != -1;
}

/*! @brief Obtain Local IP
    @return String. */
String Unit_PoESP32::obtainLocalIP() {
  sendCMD("AT+CIFSR");
  _readstr = waitMsg(1000, "192", "ERROR");
  int idx = _readstr.indexOf("+CIFSR:ETHIP,\"");
  if (idx != -1) {
    _localIP = _readstr.substring(idx + 14, _readstr.length() - 1);
    idx = _localIP.indexOf("\"");
    _localIP = idx != -1 ? _localIP.substring(0, idx) : _localIP = "";
  }
  return _readstr;
}

/*! @brief Get Local IP Address
    @return String. */
String Unit_PoESP32::getLocalIP() {
  return _localIP;
}

/*! @brief Activate Multi Connection Mode
    @return String. */
String Unit_PoESP32::activateMUXMode() {
  sendCMD("AT+CIPMUX=1");
  _readstr = waitMsg(1000, "OK", "ERROR");
  return _readstr;
}

/*! @brief Activate TCP Server
    @return String. */
String Unit_PoESP32::activateTcpServerPort80() {
  sendCMD("AT+CIPSERVER=1,80");
  _readstr = waitMsg(1000, "OK", "ERROR");
  return _readstr;
}

/*! @brief Activate NTP Client
    @return String. */
String Unit_PoESP32::activateNTPClient() {
  sendCMD("AT+CIPSNTPCFG=1,0,\"time.google.com\",\"0.pool.ntp.org\",\"1.pool.ntp.org\"");
  _readstr = waitMsg(1000, "OK", "ERROR");
  return _readstr;
}

/*! @brief Get NTP Time
    @return True if good. */
bool Unit_PoESP32::getNTPTime(struct tm *tm) {
  sendCMD("AT+CIPSNTPTIME?");
  _readstr = waitMsg(1000, "OK", "ERROR");
  if (_readstr.indexOf("OK") != -1) {
    int idx = _readstr.indexOf("+CIPSNTPTIME:");
    if (idx != -1) {
      String strTime = _readstr.substring(idx + strlen("+CIPSNTPTIME:"));
      Serial.println(strTime.c_str());
      int result = parse_asctime(strTime.c_str(), tm);
      Serial.printf("%d\n", result);
      return result == 0;
    }
  }
  return false;
}

/*! @brief Create a TCP client
    @return String. */
String Unit_PoESP32::createTCPClient(String ip, int port) {
  sendCMD("AT+CIPSTARTEX=\"TCP\",\"" + ip + "\"," + String(port));
  _readstr = waitMsg(5000, "CONNECT");
  return _readstr;
}

/*! @brief Create a SSL client
    @return String. */
String Unit_PoESP32::createSSLClient(String ip, int port) {
  sendCMD("AT+CIPSTARTEX=\"SSL\",\"" + ip + "\"," + String(port));
  _readstr = waitMsg(5000, "CONNECT");
  return _readstr;
}

/*! @brief send a TCP data
    @return True if send successfully, false otherwise. */
bool Unit_PoESP32::sendTCPData(int connectionId, uint8_t* buffer, size_t size) {
  sendCMD("AT+CIPSEND=" + String(connectionId) + "," + String(size));
  _readstr = waitMsg(500, String("OK"), String("Error"));
  _serial->write(buffer, size);
  _serial->print("");
  _readstr = waitMsg(10000, String("SEND"));
  return _readstr.indexOf("SEND OK") != -1;
}

/*! @brief send a string via TCP
    @return True if send successfully, false otherwise. */
bool Unit_PoESP32::sendTCPString(int connectionId, const char* string) {
  return sendTCPData(connectionId, (uint8_t*) string, strlen(string));
}

#endif
