//
// Created by Admin on 2017-01-09.
//

#include "aprs.h"
#include "QAPRSBase.h"
#include "stdio.h"
#include "ublox.h"
#include "config.h"
#include <string.h>
#include <stdlib.h>

#if !defined(__OPTIMIZE__)
#error "APRS Works only when optimization enabled at level at least -O2"
#endif
QAPRSBase qaprs;

void aprs_init(){
// org  qaprs.init(0, 0, (char *) APRS_CALLSIGN, (const uint8_t) APRS_SSID, (char *) "APZQAP", '0', (char *) "WIDE1-1");
  qaprs.init(0, 0, (char *) APRS_CALLSIGN, (const uint8_t) APRS_SSID, (char *) "RS41HU", '0', (char *) "WIDE1-1");

}

void aprs_timer_handler() {
  qaprs.timerInterruptHandler();
}

uint8_t aprs_is_active() {
  return qaprs.enabled;
}

void calcDMH(long x, int8_t* degrees, uint8_t* minutes, uint8_t* h_minutes) {
  uint8_t sign = (uint8_t) (x > 0 ? 1 : 0);
  if (!sign) {
    x = -(x);
  }
  *degrees = (int8_t) (x / 1000000);
  x = x - (*degrees * 1000000);
  x = (x) * 60 / 10000;
  *minutes = (uint8_t) (x / 100);
  *h_minutes = (uint8_t) (x - (*minutes * 100));
  if (!sign) {
    *degrees = -*degrees;
  }
}

void aprs_test(){
  char packet_buffer[128];
  sprintf(packet_buffer,
          (":TEST1234567890")
  );
  qaprs.sendData(packet_buffer);
}

void aprs_send_position(GPSEntry gpsData, int8_t temperature, uint16_t voltage) {
  char packet_buffer[128];
  int8_t la_degrees, lo_degrees;
  uint8_t la_minutes, la_h_minutes, lo_minutes, lo_h_minutes;
  long volts;
  uint8_t highvolts, lowvolts;

  volts = voltage;
  highvolts = abs(volts / 100);
  lowvolts = (volts - (highvolts*100));

  calcDMH(gpsData.lat_raw/10, &la_degrees, &la_minutes, &la_h_minutes);
  calcDMH(gpsData.lon_raw/10, &lo_degrees, &lo_minutes, &lo_h_minutes);

  static uint16_t aprs_packet_counter = 0;
  aprs_packet_counter ++;

  sprintf(packet_buffer,
// org         ("!%02d%02d.%02u%c/%03d%02u.%02u%cO/A=%06ld/P%dS%dT%dV%d%s"),
          ("!%02d%02d.%02u%c/%03d%02u.%02u%cO/A=%06ld/#%d, SAT:%d, tCPU:%d, Bat:%d.%02d V, %s"),
          abs(la_degrees), la_minutes, la_h_minutes,
          la_degrees > 0 ? 'N' : 'S',
          abs(lo_degrees), lo_minutes, lo_h_minutes,
          lo_degrees > 0 ? 'E' : 'W',
          (gpsData.alt_raw/1000) * 3280 / 1000,
          aprs_packet_counter,
          gpsData.sats_raw,
          temperature,
          highvolts,
          lowvolts,
          APRS_COMMENT
  );
  qaprs.sendData(packet_buffer);
}

void aprs_change_tone_time(uint16_t x) {
  qaprs._toneSendTime = x;
}
