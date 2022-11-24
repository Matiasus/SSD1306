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
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 * ---------------------------------------------------------------+
 */

// include libraries
#include <stdio.h>
#include "lib/ssd1306.h"

/**
 * @desc    Main function
 *
 * @param   void
 *
 * @return  int
 */
int main(void)
{
/*
  uint8_t x = 0;
  uint8_t y = 0;
*/
  // init ssd1306
  SSD1306_Init ();
  // clear screen
  SSD1306_ClearScreen ();
  // draw string
  SSD1306_DrawString ("SSD1306 MATIASUS");
/*
  while (x < 10) {

    SSD1306_DrawPixel (x++,y++);
  }
*/
  // return value
  return 0;
}
