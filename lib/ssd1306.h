/** 
 * ---------------------------------------------------------------+ 
 * @desc        SSD1306 OLED Driver
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @update      19.07.2021
 * @file        ssd1306.h
 * @version     2.0
 * @tested      AVR Atmega328
 *
 * @depend      font.h, twi.h
 * ---------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 * ---------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

  // Success
  // -------------------------------------------
  #define SSD1306_SUCCESS           0

  // Error
  // -------------------------------------------
  #define SSD1306_ERROR             1

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
  // -----------------------------------
  #define CLEAR_COLOR               0x00

  // Init Status
  // -----------------------------------
  #define INIT_STATUS               0xFF

  // AREA definition
  // -----------------------------------
  #define START_PAGE_ADDR           0
  #define END_PAGE_ADDR             7
  #define START_COLUMN_ADDR         0
  #define END_COLUMN_ADDR           127

  #define CACHE_SIZE_MEM            (1 + END_PAGE_ADDR) * (1 + END_COLUMN_ADDR)

  #define MAX_X                     END_COLUMN_ADDR
  #define MAX_Y                     (END_PAGE_ADDR+1)*8

  // @var set area
  unsigned int _counter;

  /**
   * @desc    SSD1306 Init
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_Init (uint8_t);

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
   * @desc    SSD1306 Clear screen
   *
   * @param   void
   *
   * @return  void
   */
  void SSD1306_ClearScreen (void);

  /**
   * @desc    SSD1306 Normal colors
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_NormalScreen (uint8_t);

  /**
   * @desc    SSD1306 Inverse colors
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_InverseScreen (uint8_t);

  /**
   * @desc    SSD1306 Update screen
   *
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_UpdateScreen (uint8_t);

  /**
   * @desc    SSD1306 Update text position
   *
   * @param   void
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_UpdTxtPosition (void);

  /**
   * @desc    SSD1306 Set position
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  void
   */
  void SSD1306_SetPosition (uint8_t, uint8_t);

  /**
   * @desc    SSD1306 Draw character
   *
   * @param   char
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawChar (char);

  /**
   * @desc    SSD1306 Draw string
   *
   * @param   char *
   *
   * @return  void
   */
  void SSD1306_DrawString (char *);

  /**
   * @desc    Draw pixel
   *
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawPixel (uint8_t, uint8_t);

  /**
   * @desc    Draw line
   *  
   * @param   uint8_t
   * @param   uint8_t
   * @param   uint8_t
   * @param   uint8_t
   *
   * @return  uint8_t
   */
  uint8_t SSD1306_DrawLine (uint8_t, uint8_t, uint8_t, uint8_t);

#endif
