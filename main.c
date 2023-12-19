/** 
 * --------------------------------------------------------------------------------------+  
 * @name        OLED SSD1306 example
 * --------------------------------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @file        main.c
 * @version     2.0.0
 * @test        AVR Atmega328p
 *
 * @depend      lib/ssd1306.h
 * --------------------------------------------------------------------------------------+ 
 * @brief       Version 1.0.0 -> applicable for 1 display
 *              Version 2.0.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0.0 -> simplified alphanumeric version for 1 display
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
  // LCD INIT
  // ------------------------------------------------------------------------------------- 
  SSD1306_Init (SSD1306_ADDR);                                    // 0x3C

  // DRAWING
  // -------------------------------------------------------------------------------------
  SSD1306_ClearScreen ();                                         // clear screen
  SSD1306_DrawLine (0, MAX_X, 4, 4);                              // draw line
  SSD1306_SetPosition (7, 1);                                     // set position
  SSD1306_DrawString ("SSD1306 OLED DRIVER");                     // draw string
  SSD1306_DrawLine (0, MAX_X, 18, 18);                            // draw line
  SSD1306_SetPosition (40, 3);                                    // set position
  SSD1306_DrawString ("MATIASUS");                                // draw string
  SSD1306_SetPosition (53, 5);                                    // set position
  SSD1306_DrawString ("2021");                                    // draw string
  SSD1306_UpdateScreen (SSD1306_ADDR);                            // update

  _delay_ms (1000);
  SSD1306_InverseScreen (SSD1306_ADDR);

  _delay_ms (1000);
  SSD1306_NormalScreen (SSD1306_ADDR);

  // RETURN
  // -------------------------------------------------------------------------------------
  return 0;
}