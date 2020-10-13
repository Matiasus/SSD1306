/** 
 * -------------------------------------------------------------+ 
 * @desc        OLED SSD1306 example
 * -------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @file        main.c
 * @tested      AVR Atmega16
 *
 * @depend      ssd1306.h
 * -------------------------------------------------------------+ 
 */

// include libraries
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
  // init ssd1306
  SSD1306_Init();

  // clear screen
  SSD1306_ClearScreen();

  // set position
  SSD1306_SetPosition(5, 0);

  // draw string
  SSD1306_DrawString(" 0.96\" OLED SSD1306");

  // set position
  SSD1306_SetPosition(30, 1);

  // draw string
  SSD1306_DrawString("by Matiasus");

  // return value
  return 0;
}
