/** 
 * ---------------------------------------------------------------+ 
 * @desc        SSD1306 OLED Driver
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @file        ssd1306.h
 * @tested      AVR Atmega16
 *
 * @depend      twi library
 * ---------------------------------------------------------------+
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

  #define SSD1306_SUCCESS 0

  // Address definition
  // -----------------------------------
  #define SSD1306_ADDRESS           0x3C

  // Command definition
  // -----------------------------------
  #define SSD1306_COMMAND           0x80  // Continuation bit=1, D/C=0; 1000 0000
  #define SSD1306_COMMAND_STREAM    0x00  // Continuation bit=0, D/C=0; 0000 0000
  #define SSD1306_DATA              0xC0  // Continuation bit=1, D/C=1; 1100 0000
  #define SSD1306_DATA_STREAM       0x40  // Continuation bit=0, D/C=1; 0100 0000

  #define SSD1306_SET_MUX_RATIO     0xA8
  #define SSD1306_DISPLAY_OFFSET    0xD3
  #define SSD1306_DISPLAY_ON        0xAF
  #define SSD1306_DISPLAY_OFF       0xAE
  #define SSD1306_DIS_ENT_DISP_ON   0xA4
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
  #define SSD1306_COM_SCAN_DIR_OP   0xC0
  #define SSD1306_COM_PIN_CONF      0xDA
  #define SSD1306_SET_CONTRAST      0x81
  #define SSD1306_SET_OSC_FREQ      0xD5
  #define SSD1306_SET_CHAR_REG      0x8D

  // Characters definition
  // -----------------------------------
  // number of columns for chars
  #define CHARS_COLS_LENGTH            5

  // Clear Color
  // -----------------------------------
  #define CLEAR_COLOR               0x00

  // Init Status
  // -----------------------------------
  #define INIT_STATUS               0xFF

  // AREA definition
  // -----------------------------------
  #define START_PAGE_ADDR              0
  #define END_PAGE_ADDR                7
  #define START_COLUMN_ADDR            0
  #define END_COLUMN_ADDR            127
  // @var set area
  volatile unsigned int set_area;

  /**
   * @desc    SSD1306 Init
   *
   * @param   void
   *
   * @return  char
   */
  char SSD1306_Init(void);

  /**
   * @desc    SSD1306 Send Start and SLAW request
   *
   * @param   void
   *
   * @return  char
   */
  char SSD1306_Send_StartAndSLAW(void);

  /**
   * @desc    SSD1306 Send Command
   *
   * @param   char
   *
   * @return  char
   */
  char SSD1306_Send_Command(char);

  /**
   * @desc    SSD1306 Clear Screen
   *
   * @param   void
   *
   * @return  char
   */
  char SSD1306_ClearScreen(void);

  /**
   * @desc    SSD1306 Set Position
   *
   * @param   char
   * @param   char
   *
   * @return  char
   */
  char SSD1306_SetPosition(char, char);

  /**
   * @desc    SSD1306 Draw Character
   *
   * @param   char
   *
   * @return  char
   */
  char SSD1306_DrawChar(char);

  /**
   * @desc    SSD1306 Draw String
   *
   * @param   char *
   *
   * @return  char
   */
  char SSD1306_DrawString(char *);

#endif
