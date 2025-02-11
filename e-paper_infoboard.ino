#define DBG_WIFI    // because "DEBUG_WIFI" defined in a WiFiClient library

#include "DEV_Config.h"
//#include "EPD.h"
#include "utility/Debug.h"
#ifndef __DEBUG_H
#define Debug(__info)
#endif
#include <WiFi.h>
#include <HTTPClient.h>
#include <NetworkClientSecure.h>
#include "utility/EPD_7in5_V2.h"
#include "GUI_Paint.h"
//#include "ImageData.h"
#include <stdlib.h>

#include "TickTwo.h"    // https://github.com/sstaub/TickTwo

#include "config.h"

UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8 ) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
UBYTE *BlackImage;
UBYTE *UsualImage;
//unsigned long curr_crc = 0;
//unsigned long prev_crc = 0;
unsigned long prev_timestamp = 0;

void check_updates();

//TickTwo timer1( check_updates, 300000);
TickTwo timer1( check_updates, 60000);

void setup (){
  DEV_Module_Init();
  DEV_Delay_ms(200);
  Debug("\r\n\r\nSerial debug started\r\n");
  wifi_init();
  timer1.start();
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply for black memory...\r\n");
    while (1);
  }
  if ((UsualImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply memory for usual image...\r\n");
    while (1);
  }
  // Debug("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
}

void loop () {
  timer1.update();
}

void check_updates(){
unsigned long new_timestamp = 0;
bool rc = false;

  Debug("Regular update\r\n");
  new_timestamp = read_info_file();
  if ( new_timestamp > 0 and new_timestamp != prev_timestamp ) {
//    curr_crc = ram_crc();
//    Serial.printf("CRC = %u prev CRC = %u\r\n", curr_crc, prev_crc);
//    if ( curr_crc != prev_crc ) {
//    prev_crc = curr_crc;
    Serial.printf("newT = %u prevT = %u\r\n", new_timestamp, prev_timestamp);
    rc = read_data();
    if ( rc ) {
      prev_timestamp = new_timestamp;
      Debug("Need to redraw screen\r\n");
      write_to_display();
    }
  }
}

void write_to_display (){

  Debug("e-Paper Init and Clear...\r\n");
  EPD_7IN5_V2_Init();
  EPD_7IN5_V2_Clear();
  DEV_Delay_ms(500);

  Debug("EPD_7IN5_V2_Display\r\n");
  EPD_7IN5_V2_Display(UsualImage);
  DEV_Delay_ms(500);

  Debug("Goto Sleep...\r\n");
  EPD_7IN5_V2_Sleep();
  free(BlackImage);
  BlackImage = NULL;
}
