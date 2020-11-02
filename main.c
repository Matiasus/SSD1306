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
  // init ssd1306
  SSD1306_Init();

  // clear screen
  SSD1306_ClearScreen();
  // draw line
  SSD1306_DrawLineHorizontal(4, 4, 115);
  // set position x, y
  SSD1306_SetPosition(5, 1);
  // draw string
  SSD1306_DrawString("SSD1306 OLED DRIVER");
  // draw line
  SSD1306_DrawLineHorizontal(4, 18, 115);
  // update
  SSD1306_UpdateScreen();

  // return value
  return 0;
}
