/**
 * -------------------------------------------------------------------------------------+
 * @desc        SSD1306 OLED Driver
 * -------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @update      15.12.2022
 * @file        ssd1306.h
 * @version     3.0.0
 * @tested      AVR Atmega328p
 *
 * @depend      font.h, twi.h
 * -------------------------------------------------------------------------------------+
 * @descr       Version 1.0.0 -> applicable for 1 display
 *              Version 2.0.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0.0 -> simplified alphanumeric version for 1 display
 * -------------------------------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

  // includes
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

  #define SSD1306_SET_MUX_RATIO     0xA8
  #define SSD1306_DISPLAY_OFFSET    0xD3
  #define SSD1306_DISPLAY_ON        0xAF
  #define SSD1306_DISPLAY_OFF       0xAE
  #define SSD1306_DIS_ENT_DISP_ON   0xA4
  #define SSD1306_DIS_IGNORE_RAM    0xA5
  #define SSD1306_DIS_NORMAL        0xA6
  #define SSD1306_DIS_INVERSE       0xA7
  #define SSD1306_DEACT_SCROLL      0x2E
  #define SSD1306_ACTIVE_SCROLL     0x2F
  #define SSD1306_SET_START_LINE    0x40
  #define SSD1306_MEMORY_ADDR_MODE  0x20
  #define SSD1306_SET_COLUMN_ADDR   0x21
  #define SSD1306_SET_PAGE_ADDR     0x22
  #define SSD1306_SEG_REMAP         0xA0
  #define SSD1306_SEG_REMAP_OP      0xA1
  #define SSD1306_COM_SCAN_DIR      0xC0
  #define SSD1306_COM_SCAN_DIR_OP   0xC8
  #define SSD1306_COM_PIN_CONF      0xDA
  #define SSD1306_SET_CONTRAST      0x81
  #define SSD1306_SET_OSC_FREQ      0xD5
  #define SSD1306_SET_CHAR_REG      0x8D
  #define SSD1306_SET_PRECHARGE     0xD9
  #define SSD1306_VCOM_DESELECT     0xDB

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

  /**
   * @desc    SSD1306 Send Start and SLAW request
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Send_StartAndSLAW (uint8_t);

  /**
   * @desc    SSD1306 Send command
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Send_Command (uint8_t);

  /**
   * @desc    SSD1306 Init
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Init (void);

  /**
   * @desc    SSD1306 Clear screen
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_ClearScreen (void);

  /**
   * @desc    SSD1306 Normal colors
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_NormalScreen (void);

  /**
   * @desc    SSD1306 Inverse colors
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_InverseScreen (void);

  /**
   * @desc    SSD1306 Update text position
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_UpdatePosition (uint8_t, uint8_t);

  /**
   * @desc    SSD1306 Set position
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_SetPosition (uint8_t, uint8_t);

  /**
   * @desc    SSD1306 Set window
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
   * @desc    SSD1306 Draw character
   *
   * @param   char
   * @param   enum E_Font
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawChar (char, enum E_Font);

  /**
   * @desc    SSD1306 Draw string
   *
   * @param   char *
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawString (char *, enum E_Font);

#endif
