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
 * @version     3.0
 * @tested      AVR Atmega328p
 *
 * @depend      lib/ssd1306.h
 * --------------------------------------------------------------------------------------+ 
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> less RAM requirement but with few limitation
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
  uint8_t i = 0;
  
  SSD1306_Init ();
  SSD1306_ClearScreen ();

  while (i < MAX_Y) {
    SSD1306_DrawLine (0,MAX_X,i,i);
    i++;
  }

  SSD1306_SetPosition (25, 0) ;
  SSD1306_DrawString ("SSD1306", BOLD);

  SSD1306_SetPosition (4, 2) ;
  SSD1306_DrawString ("by ", NORMAL);
  SSD1306_DrawString ("MATIASUS", NORMAL | UNDERLINE);
  SSD1306_DrawString (" (C) 2022", NORMAL);

  // return value
  return 0;
}
