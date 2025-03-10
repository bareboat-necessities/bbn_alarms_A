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
    String waitMsg(unsigned long time = 5, const char* expect_resp1 = NULL, const char* expect_resp2 = NULL);
    void sendCMD(String command);
    bool checkDeviceConnect();
    bool checkETHConnect();
    String obtainLocalIP();
    String getLocalIP();
    String activateMUXMode();
    String activateTcpServerPort80();
    String createTCPClient(String ip, int port);
    String createSSLClient(String ip, int port);
    bool sendTCPData(int connectionId, uint8_t *buffer, size_t size);
    bool sendTCPString(int connectionId, const char* string);
};

/*! @brief Initialize the Unit PoESP32.*/
void Unit_PoESP32::initETH(HardwareSerial* serial, unsigned long baud, uint8_t rx, uint8_t tx) {
  _serial = serial;
  _serial->begin(baud, SERIAL_8N1, rx, tx);
}

/*! @brief Waiting for a period of time to receive a message
    @return Received messages.. */
String Unit_PoESP32::waitMsg(unsigned long time, const char* expect_resp1, const char* expect_resp2) {
  String restr;
  unsigned long start = millis();
  while (1) {
    if (_serial->available() > 0) {
      String str = _serial->readString();
      restr += str;
    }
    if (expect_resp1 != NULL && restr.indexOf(expect_resp1) != -1) {
      break;
    }
    if (expect_resp2 != NULL && restr.indexOf(expect_resp2) != -1) {
      break;
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
  //Serial.println(_readstr.c_str());
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
    if (idx != -1) {
      _localIP = _localIP.substring(0, idx);
    } else {
      _localIP = "";
    }
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

/*! @brief Create a TCP client
    @return String. */
String Unit_PoESP32::createTCPClient(String ip, int port) {
  //sendCMD("AT+CIPCLOSE");
  //delay(500);
  sendCMD("AT+CIPSTARTEX=\"TCP\",\"" + ip + "\"," + String(port));
  _readstr = waitMsg(5000, "CONNECT");
  //Serial.println(_readstr.c_str());
  return _readstr;
}

/*! @brief Create a SSL client
    @return String. */
String Unit_PoESP32::createSSLClient(String ip, int port) {
  //sendCMD("AT+CIPCLOSE");
  //delay(500);
  sendCMD("AT+CIPSTARTEX=\"SSL\",\"" + ip + "\"," + String(port));
  _readstr = waitMsg(5000, "CONNECT");
  //Serial.println(_readstr.c_str());
  return _readstr;
}

/*! @brief send a TCP data
    @return True if send successfully, false otherwise. */
bool Unit_PoESP32::sendTCPData(int connectionId, uint8_t* buffer, size_t size) {
  sendCMD("AT+CIPSEND=" + String(connectionId) + "," + String(size));
  waitMsg(100, "OK");
  _serial->write(buffer, size);
  _serial->print("");
  //_serial->flush();
  _readstr = waitMsg(500, "SEND");
  //Serial.println(_readstr.c_str());
  return _readstr.indexOf("SEND OK") != -1;
}

/*! @brief send a string via TCP
    @return True if send successfully, false otherwise. */
bool Unit_PoESP32::sendTCPString(int connectionId, const char* string) {
  return sendTCPData(connectionId, (uint8_t*) string, strlen(string));
}

#endif
