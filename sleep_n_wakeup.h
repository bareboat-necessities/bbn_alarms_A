#ifndef sleep_n_wakeup_h
#define sleep_n_wakeup_h

#define uS_TO_S_FACTOR 1000000

#define SLEEP_DURATION (15 * 60 * uS_TO_S_FACTOR)

void cat_nap() {
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION);
  esp_deep_sleep_start();
}

#endif
