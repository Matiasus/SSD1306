/** 
 * ---------------------------------------------------------------+ 
 * @desc        OLED SSD1306 example
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @update      19.07.2021
 * @file        main.c
 * @tested      AVR Atmega328p
 *
 * @depend      ssd1306.h
 * ---------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> applicable for more than 1 display
 * ---------------------------------------------------------------+
 */

// include libraries
#include <stdio.h>
#include "lib/ssd1306.h"

/**
 * @desc    Main
 *
 * @param   void
 *
 * @return  unsigned char
 */
int main(void)
{
  uint8_t addr = SSD1306_ADDRESS;

  // init ssd1306
  SSD1306_Init (addr);

  // clear screen
  SSD1306_ClearScreen (addr);
  // draw line
  //SSD1306_DrawLineHorizontal (addr, 4, 4, 115);
  // set position x, y
  //SSD1306_SetPosition (addr, 1, 1);

  //for (char i = 1; i < MAX_Y; i++) {
    SSD1306_DrawPixel (addr, 0, 0);
    SSD1306_DrawPixel (addr, 0, 1);
    SSD1306_DrawPixel (addr, 0, 2);
    SSD1306_DrawPixel (addr, 1, 1);
    SSD1306_DrawPixel (addr, 2, 2);
    SSD1306_DrawPixel (addr, 3, 3);
    SSD1306_DrawPixel (addr, 4, 4);
    //SSD1306_DrawLine (addr, 0, MAX_X-1, MAX_Y-1, MAX_Y-1);
  //}

  // draw string
  //SSD1306_DrawString (addr, "SSD1306 OLED DRIVER");
  // draw line
  //SSD1306_DrawLineHorizontal (addr, 4, 18, 115);
  // update
  //SSD1306_UpdateScreen (addr);

  // return value
  return 0;
}
