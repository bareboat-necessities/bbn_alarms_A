#ifndef conf_store_h
#define conf_store_h

#include <Preferences.h>

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

void save_last_hearbeat_time(unsigned long epoch) {
  preferences.end();
  preferences.begin("msg-time", false);
  preferences.putLong("hb", epoch);
  preferences.end();
}

unsigned long get_last_hearbeat_time() {
  preferences.begin("msg-time", false);
  unsigned long hb = preferences.getLong("hb");
  preferences.end();
  return hb;
}

#endif
