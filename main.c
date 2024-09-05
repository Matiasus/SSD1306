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
 * @version     3.0.0
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
  SSD1306_Init (SSD1306_ADDR);
  SSD1306_ClearScreen ();

  SSD1306_DrawLineHorizontal (0, 0, END_COLUMN_ADDR, 0x03);
  SSD1306_DrawLineHorizontal (0, 2, END_COLUMN_ADDR, 0x18);
  SSD1306_DrawLineHorizontal (0, END_PAGE_ADDR, END_COLUMN_ADDR, 0x80);
  SSD1306_DrawLineVertical (0, 0, END_PAGE_ADDR);
  SSD1306_DrawLineVertical (END_COLUMN_ADDR, 0, END_PAGE_ADDR);

  SSD1306_SetPosition (25, 1) ;
  SSD1306_DrawString ("SSD1306", BOLD);

  SSD1306_SetPosition (4, 3) ;
  SSD1306_DrawString ("by ", NORMAL);
  SSD1306_DrawString ("MATIASUS", NORMAL | UNDERLINE);
  SSD1306_DrawString (" (C) 2024", NORMAL);

  // return value
  return 0;
}
