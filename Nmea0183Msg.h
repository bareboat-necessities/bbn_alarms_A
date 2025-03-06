#ifndef Nmea0183Msg_h
#define Nmea0183Msg_h

/*
   NMEA 0183 Messages
   
   Example:
   
    TXT - Text Transmission 
    For the transmission of short text messages, longer text messages may be transmitted by using multiple 
    sentences. Nmea0183Msg_h
     
    $--TXT,x1,x2,x3,c--c*hh<CR><LF> 
           x1    Total number of sentences 1, 01 to 99 
           x2    Sentence sequence number, 01 to 99 
           x3    Text identifier
           c--c  Text message
    
     $GPTXT,01,01,25,DR MODE - ANTENNA FAULT^21*38<CR><LF>
 */
#include "NmeaChecksum.h"

void gen_nmea0183_msg(const char *nmea_fmt, const char *value) {
  char nmea_part[82];
  snprintf(nmea_part, 76, nmea_fmt, value);
  int checksum = nmea0183_checksum(nmea_part);
  Serial.printf("%s*%02X\r\n", nmea_part, checksum);
}

#endif
