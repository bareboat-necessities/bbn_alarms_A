#ifndef gpio_jsn_sr04t_h
#define gpio_jsn_sr04t_h

#include <NewPing.h>

#include "NmeaXDR.h"

#define NEWPING_TRIGGER_PIN  G7  // Pin tied to trigger pin on the ultrasonic sensor.
#define NEWPING_ECHO_PIN     G8  // Pin tied to echo pin on the ultrasonic sensor.
#define NEWPING_MAX_DISTANCE 100 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing gpio_jsn_sr04t_sonar(NEWPING_TRIGGER_PIN, NEWPING_ECHO_PIN, NEWPING_MAX_DISTANCE); // NewPing setup of pins and maximum distance.

float gpio_jsn_sr04t_distance_cm(float temp = 28.0) {
  // temp - Temperature in Celsius (this value would probably come from a temperature sensor).
  float factor = sqrt(1 + temp / 273.15) / 60.368; // Speed of sound calculation based on temperature.
  float distance_cm = (float)gpio_jsn_sr04t_sonar.ping_median(5) * factor;
  return distance_cm;
}

void gpio_jsn_sr04t_report() {
  float distance_cm = gpio_jsn_sr04t_distance_cm();
  if (fabs(distance_cm) > 0.001) {
    gen_nmea0183_xdr("$BBXDR,D,%.2f,M,Range_JSN_S04T", distance_cm / 100.0);
  }
}

void gpio_jsn_sr04t_try_init() {
  pinMode(NEWPING_TRIGGER_PIN, OUTPUT);
  pinMode(NEWPING_ECHO_PIN, INPUT);
  app.onRepeat(3000, []() {
    gpio_jsn_sr04t_report();
  });
}

#endif
