/**
 * -------------------------------------------------------------------------------------+
 * @brief       SSD1306 OLED Driver
 * -------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @update      21.11.2022
 * @file        ssd1306.h
 * @version     3.0
 * @tested      AVR Atmega328p
 *
 * @depend      font.h, twi.h
 * -------------------------------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> simplified alphanumeric version
 * -------------------------------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

  // includes
  #include <util/delay.h>
  #include "font.h"
  #include "twi.h"

  // Success / Error
  // ------------------------------------------------------------------------------------
  #define SSD1306_SUCCESS           0
  #define SSD1306_ERROR             1

  // Address definition
  // ------------------------------------------------------------------------------------
  #define SSD1306_ADDR              0x3C

  // Command definition
  // ------------------------------------------------------------------------------------
  #define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
  #define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
  #define SSD1306_DATA              0xC0  // Continuation bit=1, D/C=1; 1100 0000
  #define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000

  #define SSD1306_SET_MUX_RATIO     0xA8  // Set MUX ratio to N+1 MUX, N=A[5:0] : from 16MUX to 64MUX
  #define SSD1306_DISPLAY_OFFSET    0xD3  // Set Display Offset
  #define SSD1306_DISPLAY_ON        0xAF  // Display ON in normal mode
  #define SSD1306_DISPLAY_OFF       0xAE  // Display OFF (sleep mode)
  #define SSD1306_DIS_ENT_DISP_ON   0xA4  // Entire Display ON, Output ignores RAM content
  #define SSD1306_DIS_IGNORE_RAM    0xA5  // Resume to RAM content display, Output follows RAM content
  #define SSD1306_DIS_NORMAL        0xA6  // Normal display, 0 in RAM: OFF in display panel, 1 in RAM: ON in display panel
  #define SSD1306_DIS_INVERSE       0xA7  // Inverse display, 0 in RAM: ON in display panel, 1 in RAM: OFF in display panel
  #define SSD1306_DEACT_SCROLL      0x2E  // Stop scrolling that is configured by command 26h/27h/29h/2Ah
  #define SSD1306_ACTIVE_SCROLL     0x2F  // Start scrolling that is configured by the scrolling setup commands:26h/27h/29h/2Ah
  #define SSD1306_SCROLL_HOR_RIGHT  0x26  // Horizontal Right Horizontal Scroll
  #define SSD1306_SCROLL_HOR_LEFT   0x27  // Horizontal Left Horizontal Scroll
  #define SSD1306_SET_START_LINE    0x40  // Set Display Start Line
  #define SSD1306_MEMORY_ADDR_MODE  0x20  // Set Memory, Addressing Mode
  #define SSD1306_SET_COLUMN_ADDR   0x21  // Set Column Address
  #define SSD1306_SET_PAGE_ADDR     0x22  // Set Page Address
  #define SSD1306_SEG_REMAP         0xA0  // Set Segment Re-map, X[0]=0b column address 0 is mapped to SEG0
  #define SSD1306_SEG_REMAP_OP      0xA1  // Set Segment Re-map, X[0]=1b: column address 127 is mapped to SEG0
  #define SSD1306_COM_SCAN_DIR      0xC0  // Set COM Output, X[3]=0b: normal mode (RESET) Scan from COM0 to COM[N –1], e N is the Multiplex ratio
  #define SSD1306_COM_SCAN_DIR_OP   0xC8  // Set COM Output, X[3]=1b: remapped mode. Scan from COM[N-1] to COM0, e N is the Multiplex ratio
  #define SSD1306_COM_PIN_CONF      0xDA  // Set COM Pins Hardware Configuration, 
                                          // A[4]=0b, Sequential COM pin configuration, A[4]=1b(RESET), Alternative COM pin configuration
                                          // A[5]=0b(RESET), Disable COM Left/Right remap, A[5]=1b, Enable COM Left/Right remap                                        
  #define SSD1306_SET_CONTRAST      0x81  // Set Contrast Control, Double byte command to select 1 to 256 contrast steps, increases as the value increases
  #define SSD1306_SET_OSC_FREQ      0xD5  // Set Display Clock Divide Ratio/Oscillator Frequency
                                          // A[3:0] : Define the divide ratio (D) of the  display clocks (DCLK): Divide ratio= A[3:0] + 1, RESET is 0000b (divide ratio = 1) 
                                          // A[7:4] : Set the Oscillator Frequency, FOSC. Oscillator Frequency increases with the value of A[7:4] and vice versa. RESET is 1000b
  #define SSD1306_SET_CHAR_REG      0x8D  // Charge Pump Setting, A[2] = 0b, Disable charge pump(RESET), A[2] = 1b, Enable charge pump during display on 
                                          // The Charge Pump must be enabled by the following command:
                                          // 8Dh ; Charge Pump Setting
                                          // 14h ; Enable Charge Pump
                                          // AFh; Display ON
  #define SSD1306_SET_PRECHARGE     0xD9  // Set Pre-charge Period
  #define SSD1306_VCOM_DESELECT     0xDB  // Set VCOMH Deselect Level
  #define SSD1306_NOP               0xE3  // No operation
  #define SSD1306_RESET             0xE4  // Maybe SW RESET, @source https://github.com/SmingHub/Sming/issues/501

  #define SSD1306_VERTICAL_MODE     0x01
  #define SSD1306_HORIZONTAL_MODE   0x00

  // Clear Color
  // ------------------------------------------------------------------------------------
  #define CLEAR_COLOR               0x00

  // Init Status
  // ------------------------------------------------------------------------------------
  #define INIT_STATUS               0xFF

  // AREA definition
  // ------------------------------------------------------------------------------------
  #define START_PAGE_ADDR           0
  #define END_PAGE_ADDR             7     // 7 for 128x64, 3 for 128x32 version
  #define START_COLUMN_ADDR         0
  #define END_COLUMN_ADDR           127
  #define RAM_X_END                 END_COLUMN_ADDR + 1
  #define RAM_Y_END                 END_PAGE_ADDR + 1

  #define CACHE_SIZE_MEM            (1 + END_PAGE_ADDR) * (1 + END_COLUMN_ADDR)

  #define MAX_X                     END_COLUMN_ADDR
  #define MAX_Y                     (END_PAGE_ADDR + 1) * 8

  // @enum
  enum E_Font {
    NORMAL = 0x00,
    BOLD = 0x01,
    UNDERLINE = 0x10
  };

  // @enum
  enum E_Line {
    TOP = 0x01,
    TOPDOUBLE = 0x03,
    MIDDLE = 0x01,
    MIDDLEDOUBLE = 0x18,
    BOTTOM = 0x80,
    BOTTOMDOUBLE = 0xC0
  };

  /**
   * @brief   SSD1306 Send Start and SLAW request
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Send_StartAndSLAW (uint8_t);

  /**
   * @brief   SSD1306 Send command
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Send_Command (uint8_t);

  /**
   * @brief   SSD1306 Init
   *
   * @param   uint8_t address
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Init (uint8_t);

  /**
   * @brief   SSD1306 Clear screen
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_ClearScreen (void);

  /**
   * @desc    SSD1306 Clear Page
   *
   * @param   uint8_t page
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_ClearPage (uint8_t);

  /**
   * @desc    SSD1306 Clear screen
   *
   * @param   uint8_t page
   * @param   uint8_t margin
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_ClearPageFromToEnd (uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Normal colors
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_NormalScreen (void);

  /**
   * @brief   SSD1306 Inverse colors
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_InverseScreen (void);

  /**
   * @brief   SSD1306 Update text position
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_UpdatePosition (uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Set position
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_SetPosition (uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Set window
   *
   * @param   uint8_t column -> 0 ... 127
   * @param   uint8_t column -> 0 ... 127
   * @param   uint8_t page -> 0 ... 7
   * @param   uint8_t page -> 0 ... 7
   *
   * @return  void
   */
  uint8_t SSD1306_SetWindow (uint8_t, uint8_t, uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Draw character
   *
   * @param   char
   * @param   enum E_Font
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawChar (char, enum E_Font);

  /**
   * @brief   SSD1306 Draw string
   *
   * @param   char *
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawString (char *, enum E_Font);

  /**
   * @brief   SSD1306 Draw line horizontal
   *
   * @param   uint8_t x
   * @param   uint8_t y
   * @param   uint8_t width
   * @param   enum E_line {TOP, TOPDOUBLE, ... BOTTOM, BOTTOMDOUBLE} see enum E_Line
   * 
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawLineHorizontal (uint8_t, uint8_t, uint8_t, enum E_Line);

  /**
   * @brief   SSD1306 Draw line vertical
   *
   * @param   uint8_t x {0 -> END_COLUMN_ADDRESS}
   * @param   uint8_t y {0 -> END_PAGE_ADDR}
   * @param   uint8_t height
   * 
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawLineVertical (uint8_t, uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Horizontal Scroll Start
   *
   * @param   uint8_t address
   * @param   uint8_t start page
   * @param   uint8_t end page
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_HorizontalScroll (uint8_t, uint8_t, uint8_t);

  /**
   * @brief   SSD1306 Horizontal Scroll Stop
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_HorizontalScrollStop (uint8_t);

#endif