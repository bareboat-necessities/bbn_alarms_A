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

#endif
