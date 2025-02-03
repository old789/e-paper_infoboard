#include "DEV_Config.h"
//#include "EPD.h"
#include "utility/Debug.h"
#ifndef __DEBUG_H
#define Debug(__info)
#endif
#include "utility/EPD_7in5_V2.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include <stdlib.h>

UBYTE *BlackImage;

void setup (){
  DEV_Module_Init();
  DEV_Delay_ms(200);
  Debug("\r\n\r\nSerial debug started");

  Debug("e-Paper Init and Clear...\r\m");
  EPD_7IN5_V2_Init();
  EPD_7IN5_V2_Clear();
  DEV_Delay_ms(500);

  UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8 ) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
  if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
    Debug("Failed to apply for black memory...");
    while (1);
  }
  // Debug("Paint_NewImage\r\n");
  Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, WHITE);
  //DEV_Delay_ms(5000);

  Debug("Paint_DrawBitMap\r\n");
  Paint_DrawBitMap(pict);
  //DEV_Delay_ms(5000);

  Debug("EPD_7IN5_V2_Display\r\n");
  EPD_7IN5_V2_Display(BlackImage);
  DEV_Delay_ms(500);

  Debug("Goto Sleep...\r\n");
  EPD_7IN5_V2_Sleep();
  free(BlackImage);
  BlackImage = NULL;
}

void loop () {
//
}
