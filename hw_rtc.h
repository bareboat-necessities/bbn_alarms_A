#ifndef HW_RTC_H
#define HW_RTC_H

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

  void rtc_begin() {
    M5.Rtc.begin();
  }

  void rtc_set(struct tm *timeinfo) {
    time_t time = mktime(timeinfo);
    M5.Rtc.setDateTime(time);  // writes the  time to the (RTC) real time clock.
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
