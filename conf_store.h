#ifndef conf_store_h
#define conf_store_h

#include <Preferences.h>

#define VOLTAGE_ALARM_THRESHOLD  11.7
#define LEVEL_CM_ALARM_THRESHOLD 25.0

// config store.
Preferences preferences;

void save_settings(String phone, String apiKey) {
  preferences.end();
  preferences.begin("msg-cfg", false);
  preferences.putString("phone", phone);
  preferences.putString("apiKey", apiKey);
  preferences.end();
}

void restore_settings() {
  preferences.begin("msg-cfg", false);
  phoneNumber = preferences.getString("phone");
  apiKey = preferences.getString("apiKey");
  preferences.end();
}

void save_voltageThreshold(float voltage_threshold) {
  preferences.end();
  preferences.begin("cfg_volt", false);
  preferences.putFloat("V", voltage_threshold);
  preferences.end();
}

float get_voltageThreshold() {
  preferences.end();
  preferences.begin("cfg_volt", false);
  float value = preferences.getFloat("V", VOLTAGE_ALARM_THRESHOLD);
  preferences.end();
  return value;
}

void save_bilgeThreshold(float bilge_threshold) {
  preferences.end();
  preferences.begin("cfg_bilge", false);
  preferences.putFloat("cm", bilge_threshold);
  preferences.end();
}

float get_bilgeThreshold() {
  preferences.end();
  preferences.begin("cfg_bilge", false);
  float value = preferences.getFloat("cm", LEVEL_CM_ALARM_THRESHOLD);
  preferences.end();
  return value;
}

void save_last_heartbeat_time(uint64_t epoch) {
  preferences.end();
  preferences.begin("msg-time1", false);
  preferences.putULong64("hb", epoch);
  preferences.end();
}

uint64_t get_last_heartbeat_time() {
  preferences.begin("msg-time1", false);
  uint64_t epoch = preferences.getULong64("hb");
  preferences.end();
  return epoch;
}

void save_last_alarm_time(uint64_t epoch) {
  preferences.end();
  preferences.begin("msg-time2", false);
  preferences.putULong64("alarm", epoch);
  preferences.end();
}

uint64_t get_last_alarm_time() {
  preferences.begin("msg-time2", false);
  uint64_t epoch = preferences.getULong64("alarm");
  preferences.end();
  return epoch;
}

#endif

