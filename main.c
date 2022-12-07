/** 
 * --------------------------------------------------------------------------------------+  
 * @desc        OLED SSD1306 example
 * --------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @update      19.07.2021
 * @file        main.c
 * @version     3.0
 * @tested      AVR Atmega328p
 *
 * @depend      ssd1306.h
 * --------------------------------------------------------------------------------------+ 
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> less RAM requirement but with few limitation
 * --------------------------------------------------------------------------------------+ 
 */

// include libraries
#include "lib/ssd1306.h"
#include <util/delay.h>

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
  // init ssd1306
  SSD1306_Init ();
  // clear screen
  SSD1306_ClearScreen ();
/*
  while (x < MAX_Y) {
    SSD1306_DrawLine (x,x,0,y++);
    x++;
  }
*/
  //SSD1306_DrawLine (0,MAX_X,1,1);
  //SSD1306_DrawLine (5,5,0,7);
  //SSD1306_DrawLine (7,7,2,7);
  //SSD1306_DrawLine (0,MAX_X,20,20);
  //SSD1306_DrawLine (9,9,0,9);
  //SSD1306_DrawLine (0,MAX_X,20,20);

  //SSD1306_DrawLineHorz (1,0,MAX_X);
  //SSD1306_DrawLineHorz (10,0,MAX_X);

  while (i < MAX_Y) {
    SSD1306_DrawLine (0,MAX_X-10,i,i);
    //_delay_ms (100);
    i++;
  }

  SSD1306_SetPosition (0, 0) ;
  // draw string
  SSD1306_DrawString ("SSD1306 MATIASUS Copyright (C) 2020 Marian Hrinko. Copyright (C) 2020 Marian Hrinko. Copyright (C) 2020 Marian Hrinko.");

  // return value
  return 0;
}
