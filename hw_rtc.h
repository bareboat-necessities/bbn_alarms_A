#ifndef HW_RTC_H
#define HW_RTC_H

#ifdef __cplusplus
extern "C" {
#endif

  RTC_TimeTypeDef RTCtime;
  RTC_DateTypeDef RTCdate;

  void rtc_begin() {
    M5.Rtc.begin();
  }

  void rtc_set(struct tm *timeinfo) {
    RTCdate.Year = timeinfo->tm_year;
    RTCdate.Month = timeinfo->tm_mon;
    RTCdate.Date = timeinfo->tm_mday;
    M5.Rtc.SetDate(&RTCdate);

    RTCtime.Hours = timeinfo->tm_hour;
    RTCtime.Minutes = timeinfo->tm_min;
    RTCtime.Seconds = timeinfo->tm_sec;
    M5.Rtc.SetTime(&RTCtime);  // writes the  time to the (RTC) real time clock.
  }

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
