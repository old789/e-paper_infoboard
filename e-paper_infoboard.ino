#define DBG_WIFI    // because "DEBUG_WIFI" defined in a WiFiClient library
#define DBG_SERIAL

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
#include <stdlib.h>

#include <EEPROM.h>

#include <SimpleCLI.h>  // https://github.com/SpacehuhnTech/SimpleCLI

#include "TickTwo.h"    // https://github.com/sstaub/TickTwo

#include "uptime.h"     // https://github.com/YiannisBourkelis/Uptime-Library

#define MAX_ALLOWED_INPUT 127
#define BUTTON            33

void check_updates();
void count_uptime();

SimpleCLI cli;

TickTwo timer1( check_updates, 180000);
TickTwo timer2( count_uptime, 1000);

UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8 ) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
UBYTE *UsualImage;
unsigned long prev_timestamp = 0;
char str_uptime[25] = "0d0h0m0s";
bool enable_cli = false;
bool eeprom_bad = false;

// EEPROM data
uint16_t mark = 0x55aa;
char dev_name[33] = {0};
char ssid[33] = {0};
char passw[49] = {0};
char host[49] = {0};
uint16_t port = 443;
char uri1[65] = {0};
char uri2[65] = {0};
uint8_t http_auth = 0;
char http_user[33] = {0};
char http_passw[33] = {0};
uint8_t debug = 0;

#define PT_DEV_NAME         sizeof(mark)
#define PT_SSID             PT_DEV_NAME + sizeof(dev_name)
#define PT_PASSW            PT_SSID + sizeof(ssid)
#define PT_HOST             PT_PASSW + sizeof(passw)
#define PT_PORT             PT_HOST + sizeof(host)
#define PT_URI1             PT_PORT + sizeof(port)
#define PT_URI2             PT_URI1 + sizeof(uri1)
#define PT_AUTH             PT_URI2 + sizeof(uri2)
#define PT_HUSER            PT_AUTH + sizeof(http_auth)
#define PT_HPASSW           PT_HUSER + sizeof(http_user)
#define PT_DEBUG            PT_HPASSW + sizeof(http_passw)
#define PT_CRC              PT_DEBUG + sizeof(debug)
#define SIZE_EEPROM         PT_DEBUG + sizeof(debug) - 1 // PT_CRC d'not count

// Commands
Command cmdDevName;
Command cmdSsid;
Command cmdPassw;
Command cmdShow;
Command cmdHost;
Command cmdPort;
Command cmdUri1;
Command cmdUri2;
Command cmdDebug;
Command cmdHauth;
Command cmdHuser;
Command cmdHpassw;
Command cmdSave;
Command cmdReboot;
Command cmdHelp;

void setup (){
  DEV_Module_Init();
  DEV_Delay_ms(200);
  Debug("\r\n\r\nSerial debug started\r\n");
  if ((UsualImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply memory for usual image...\r\n");
    while (1);
  }

  pinMode(BUTTON,  INPUT_PULLUP);

  Debug("Init EEPROM\r\n");
  EEPROM.begin(384);

  if ( ! eeprom_read() ) {
    eeprom_bad = true;
  } else {
    if ( ! is_conf_correct() ) {
      eeprom_bad = true;
    }
  }

  if (eeprom_bad)
    Serial.println("\nEEPROM error or bad config");

  enable_cli = is_button_pressed();

  if ( enable_cli ) {
    // Command line mode
    SetSimpleCli();
    Serial.println("Usage:");
    Serial.println(cli.toString());
  } else {
    if (eeprom_bad)
      while (1);
    wifi_init();
    // check_updates();
    timer1.start();
    timer2.start();
  }
}

void loop(){
  if (enable_cli) {
    loop_cli_mode();
  }else{
    loop_usual_mode();
  }
}

void loop_usual_mode() {
  timer1.update();
  timer2.update();
}

void check_updates(){
unsigned long new_timestamp = 0;
bool rc = false;

  Serial.printf("Regular update %s\r\n", str_uptime);
  new_timestamp = read_info_file();
  if ( new_timestamp > 0 and new_timestamp != prev_timestamp ) {
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
}

void count_uptime() {
  uptime::calculateUptime();
  memset(str_uptime, 0, sizeof(str_uptime));
  sprintf( str_uptime, "%ud%uh%um%us", uptime::getDays(), uptime::getHours(), uptime::getMinutes(), uptime::getSeconds() );
}

bool is_button_pressed() {
  if ( digitalRead(BUTTON) == HIGH ) {
    return(false);
  }
  delay(2000);
  if ( digitalRead(BUTTON) == LOW ) {
    return(true);
  }
  return(false);
}
