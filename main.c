/** 
 * --------------------------------------------------------------------------------------+  
 * @desc        OLED SSD1306 example
 * --------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @update      19.07.2021
 * @file        main.c
 * @version     2.0.0
 * @tested      AVR Atmega328p
 *
 * @depend      lib/ssd1306.h
 * --------------------------------------------------------------------------------------+ 
 * @descr       Version 1.0.0 -> applicable for 1 display
 *              Version 2.0.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0.0 -> simplified alphanumeric version for 1 display
 * --------------------------------------------------------------------------------------+ 
 */

// include libraries
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
  uint8_t addr = SSD1306_ADDR;

  // init ssd1306
  SSD1306_Init (addr);

  // clear screen
  SSD1306_ClearScreen ();
  // draw line
  SSD1306_DrawLine (0, MAX_X, 4, 4);
  // set position
  SSD1306_SetPosition (7, 1);
  // draw string
  SSD1306_DrawString ("SSD1306 OLED DRIVER");
  // draw line
  SSD1306_DrawLine (0, MAX_X, 18, 18);
  // set position
  SSD1306_SetPosition (40, 3);
  // draw string
  SSD1306_DrawString ("MATIASUS");
  // set position
  SSD1306_SetPosition (53, 5);
  // draw string
  SSD1306_DrawString ("2021");
  // update
  SSD1306_UpdateScreen (addr);

  // return value
  return 0;
}
