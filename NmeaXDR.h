#ifndef NmeaXDR_h
#define NmeaXDR_h

/*
  XDR - Transducer Measurement
  https://gpsd.gitlab.io/gpsd/NMEA.html#_xdr_transducer_measurement
  https://www.eye4software.com/hydromagic/documentation/articles-and-howtos/handling-nmea0183-xdr/

  Format: $--XDR,a,x.x,a,c--c, ..... *hh<CR><LF>
  Example:

  $HCXDR,A,171,D,PITCH,A,-37,D,ROLL,G,367,,MAGX,G,2420,,MAGY,G,-8984,,MAGZ*41
  $SDXDR,C,23.15,C,WTHI*70

  Transducer Types:
     A - Angular displacement
     C - Temperature
     D - Depth/Distance
     F - Frequency
     H - Humidity
     N - Force
     P - Pressure
     R - Flow
     B - Absolute humidity
     G - Generic
     I - Current
     L - Salinity
     S - Switch, valve
     T - Tachometer
     U - Voltage
     V - Volume
     could be more

  Unit of measurement                                     
     "" - could be empty!                                 
     A - Amperes                                          
     B - Bars | Binary                                    
     C - Celsius                                          
     D - Degrees                                          
     H - Hertz                                            
     I - liters/second                                    
     K - Kelvin | Density, kg/m3 kilogram per cubic metre 
     M - Meters | Cubic Meters (m3)                       
     N - Newton                                           
     P - Percent of full range | Pascal                   
     R - RPM                                              
     S - Parts per thousand                               
     V - Volts                                            
     could be more
*/

#include "NmeaChecksum.h"

void gen_nmea0183_xdr(const char *nmea_fmt, float value) {
  char nmea_part[82];
  snprintf(nmea_part, 76, nmea_fmt, value);
  int checksum = nmea0183_checksum(nmea_part);
  Serial.printf("%s*%02X\r\n", nmea_part, checksum);
}

void gen_nmea0183_xdr_2(const char *nmea_fmt, float value, int index) {
  char nmea_part[82];
  snprintf(nmea_part, 76, nmea_fmt, value, index);
  int checksum = nmea0183_checksum(nmea_part);
  Serial.printf("%s*%02X\r\n", nmea_part, checksum);
}

void gen_nmea0183_xdr_3(const char *nmea_fmt, float value, int index1, int index2) {
  char nmea_part[82];
  snprintf(nmea_part, 76, nmea_fmt, value, index1, index2);
  int checksum = nmea0183_checksum(nmea_part);
  Serial.printf("%s*%02X\r\n", nmea_part, checksum);
}

#endif
