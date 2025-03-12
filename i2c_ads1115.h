#ifndef i2c_ads1115_h
#define i2c_ads1115_h

#include <M5_ADS1115.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

/*
   @Hardwares: M5AtomS3 Lite + Unit Vmeter ADS1115
   @Dependent Library:
   M5_ADS1115: https://github.com/m5stack/M5-ADS1115
*/

#define M5_UNIT_VMETER_I2C_ADDR             0x49
#define M5_UNIT_VMETER_EEPROM_I2C_ADDR      0x53
#define M5_UNIT_VMETER_PRESSURE_COEFFICIENT 0.015918958F

ADS1115 i2c_ads1115_sensor_0;  // I2C default bus
ADS1115 i2c_ads1115_sensor_1;  // I2C additional bus

float i2c_ads1115_voltage(ADS1115 *i2c_ads1115_sensor) {
  float resolution = i2c_ads1115_sensor->getCoefficient() / M5_UNIT_VMETER_PRESSURE_COEFFICIENT;
  float calibration_factor = i2c_ads1115_sensor->getFactoryCalibration();
  int16_t adc_raw = i2c_ads1115_sensor->getSingleConversion();
  float voltage = adc_raw * resolution * calibration_factor;
  //Serial.printf("Cal ADC:%.0f\n", adc_raw * calibration_factor);
  //Serial.printf("Cal Voltage:%.2f mV\n", voltage);
  //Serial.printf("Raw ADC:%d\n\n", adc_raw);
  return voltage;
}

void i2c_ads1115_report(ADS1115 *i2c_ads1115_sensor) {
  float voltage = i2c_ads1115_voltage(i2c_ads1115_sensor);
  if (i2c_ads1115_sensor == &i2c_ads1115_sensor_0) {
    gen_nmea0183_xdr("$BBXDR,U,%.3f,V,VOLT", voltage / 1000);     // Volt
  } else {
    gen_nmea0183_xdr("$BBXDR,U,%.3f,V,VOLT_1", voltage / 1000);   // Volt
  }
}

bool i2c_ads1115_try_init(TwoWire *wire = &Wire, uint8_t sda = SDA, uint8_t scl = SCL, uint32_t freq = 1000000UL) {
  bool i2c_ads1115_found = false;
  ADS1115 *i2c_ads1115_sensor = (wire == &Wire ? &i2c_ads1115_sensor_0 : &i2c_ads1115_sensor_1);
  for (int i = 0; i < 3; i++) {
    i2c_ads1115_found = i2c_ads1115_sensor->begin(wire, M5_UNIT_VMETER_I2C_ADDR, sda, scl, freq);
    if (i2c_ads1115_found) {
      break;
    }
    delay(10);
  }
  if (i2c_ads1115_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,VOLTAGE found ads1115 sensor at address=0x%s",
                     (String(M5_UNIT_VMETER_I2C_ADDR, HEX) + String((wire == &Wire ? " main bus" : " alt bus"))).c_str());
    i2c_ads1115_sensor->setEEPROMAddr(M5_UNIT_VMETER_EEPROM_I2C_ADDR);
    i2c_ads1115_sensor->setMode(ADS1115_MODE_SINGLESHOT);
    i2c_ads1115_sensor->setRate(ADS1115_RATE_8);
    i2c_ads1115_sensor->setGain(ADS1115_PGA_512);
    // | PGA      | Max Input Voltage(V) |
    // | PGA_6144 |        128           |
    // | PGA_4096 |        64            |
    // | PGA_2048 |        32            |
    // | PGA_512  |        16            |
    // | PGA_256  |        8             |
    app.onRepeat(3000, [i2c_ads1115_sensor]() {
      i2c_ads1115_report(i2c_ads1115_sensor);
    });
  }
  return i2c_ads1115_found;
}

#endif
