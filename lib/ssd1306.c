/**
 * --------------------------------------------------------------------------------------+
 * @desc        SSD1306 OLED Driver
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @update      06.12.2022
 * @file        ssd1306.c
 * @version     3.0
 * @tested      AVR Atmega328p
 *
 * @depend      font.h, twi.h
 * --------------------------------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> remove 'cacheMemLcd' approach
 * --------------------------------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

// @includes
#include "ssd1306.h"

// +---------------------------+
// |      Set MUX Ratio        |
// +---------------------------+
// |        0xA8, 0x3F         |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Display Offset     |
// +---------------------------+
// |        0xD3, 0x00         |
// +---------------------------+
//              |
// +---------------------------+
// |  Set Display Start Line   |
// +---------------------------+
// |          0x40             |
// +---------------------------+
//              |
// +---------------------------+
// |     Set Segment Remap     |
// +---------------------------+
// |       0xA0 / 0xA1         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Output Scan     |
// |        Direction          |
// +---------------------------+
// |       0xC0 / 0xC8         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set COM Pins hardware   |
// |       configuration       |
// +---------------------------+
// |        0xDA, 0x02         |
// +---------------------------+
//              |
// +---------------------------+
// |   Set Contrast Control    |
// +---------------------------+
// |        0x81, 0x7F         |
// +---------------------------+
//              |
// +---------------------------+
// | Disable Entire Display On |
// +---------------------------+
// |          0xA4             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Normal Display     |
// +---------------------------+
// |          0xA6             |
// +---------------------------+
//              |
// +---------------------------+
// |    Set Osc Frequency      |
// +---------------------------+
// |       0xD5, 0x80          |
// +---------------------------+
//              |
// +---------------------------+
// |    Enable charge pump     |
// |        regulator          |
// +---------------------------+
// |       0x8D, 0x14          |
// +---------------------------+
//              |
// +---------------------------+
// |        Display On         |
// +---------------------------+
// |          0xAF             |
// +---------------------------+

// @array Init command
const uint8_t INIT_SSD1306[] PROGMEM = {
  18,                                                             // number of initializers
  0, SSD1306_DISPLAY_OFF,                                         // 0xAE = Set Display OFF
  1, SSD1306_SET_MUX_RATIO, 31,                                   // 0xA8 - 64MUX for 128 x 64 version
                                                                  //      - 32MUX for 128 x 32 version
  1, SSD1306_MEMORY_ADDR_MODE, 0x00,                              // 0x20 = Set Memory Addressing Mode
                                                                  // 0x00 - Horizontal Addressing Mode
                                                                  // 0x01 - Vertical Addressing Mode
                                                                  // 0x02 - Page Addressing Mode (RESET)
  2, SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR, // 0x21 = Set Column Address, 0 - 127
  2, SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,       // 0x22 = Set Page Address, 0 - 7
  0, SSD1306_SET_START_LINE,                                      // 0x40
  1, SSD1306_DISPLAY_OFFSET, 0x00,                                // 0xD3
  0, SSD1306_SEG_REMAP_OP,                                        // 0xA0 / remap 0xA1
  0, SSD1306_COM_SCAN_DIR_OP,                                     // 0xC0 / remap 0xC8
  1, SSD1306_COM_PIN_CONF, 0x02,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
                                                                  //       0x12 - for 128 x 64 version
                                                                  //       0x02 - for 128 x 32 version
  1, SSD1306_SET_CONTRAST, 0x7F,                                  // 0x81, 0x7F - reset value (max 0xFF)
  0, SSD1306_DIS_ENT_DISP_ON,                                     // 0xA4
  0, SSD1306_DIS_NORMAL,                                          // 0xA6
  1, SSD1306_SET_OSC_FREQ, 0x80,                                  // 0xD5, 0x80 => D=1; DCLK = Fosc / D <=> DCLK = Fosc
  1, SSD1306_SET_PRECHARGE, 0xc2,                                 // 0xD9, higher value less blinking
                                                                  // 0xC2, 1st phase = 2 DCLK,  2nd phase = 13 DCLK
  1, SSD1306_VCOM_DESELECT, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  1, SSD1306_SET_CHAR_REG, 0x14,                                  // 0x8D, Enable charge pump during display on
  0, SSD1306_DISPLAY_ON                                           // 0xAF = Set Display ON
};

unsigned short int _indexCol = START_COLUMN_ADDR;                 // @var global - cache index column
unsigned short int _indexPage = START_PAGE_ADDR;                  // @var global - cache index page

/**
 * @desc    SSD1306 Init
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Init (void)
{
  const uint8_t *commands = INIT_SSD1306;                         // variables
  uint8_t no_of_commands = pgm_read_byte (commands++);            // number of commands
  uint8_t no_of_arguments;                                        // number od arguments
  uint8_t command;                                                // command
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI INIT
  // -------------------------------------------------------------------------------------
  TWI_Init ();

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // SEND COMMAND & ARGUMENTS
  // -------------------------------------------------------------------------------------
  while (no_of_commands) {                                        // commands loop
    no_of_arguments = pgm_read_byte (commands++);                 // number of arguments
    command = pgm_read_byte (commands++);                         // command
    // Send commands
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_Command (command);                      // send command
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    // Send arguments
    // -----------------------------------------------------------------------------------
    while (no_of_arguments--) {
      status = SSD1306_Send_Command (pgm_read_byte(commands++));  // send argument
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
    }
    no_of_commands--;                                             // next command
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Send Start and SLAW request
 *
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Send_StartAndSLAW (uint8_t address)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI START
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Start ();                                       // start
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI SLAW
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_SLAW (address);                            // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Send command
 *
 * @param   uint8_t command
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Send_Command (uint8_t command)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI send control byte
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_COMMAND);                    // send control byte
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (command);                            // send command
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Normal colors
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_NormalScreen (void)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_DIS_NORMAL);             // command 0xA6
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Inverse colors
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_InverseScreen (void)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_DIS_INVERSE);            // command 0xA7
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Clear screen
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_ClearScreen (void)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint16_t i = 0;                                                 // counter

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);                // send data 0x40
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  //  send clear byte to memory lcd
  // -------------------------------------------------------------------------------------
  while (i < CACHE_SIZE_MEM) {
    status = TWI_MT_Send_Data (CLEAR_COLOR);                      // send data 0x00
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    i++;                                                          // update counter
  }
  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t page -> 0 ... 7
 * @param   uint8_t page -> 0 ... 7
 *
 * @return  void
 */
uint8_t SSD1306_SetWindow (uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_COLUMN_ADDR);        // 0x21
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  status = SSD1306_Send_Command (x1);                             // start COLUMN
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  status = SSD1306_Send_Command (x2);                             // end COLUMN
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  _indexCol = x1;                                                 // update column index
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_PAGE_ADDR);          // 0x22
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  status = SSD1306_Send_Command (y1);                             // start PAGE
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  status = SSD1306_Send_Command (y2);                             // end PAGE
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  _indexPage = y1;                                                // update column index

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t page -> 0 ... 7
 *
 * @return  void
 */
uint8_t SSD1306_SetPosition (uint8_t x, uint8_t y)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint8_t x_end = END_COLUMN_ADDR;
  uint8_t y_end = END_PAGE_ADDR;
 
  status = SSD1306_SetWindow (x, x_end, y, y_end);                // end COLUMN
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Update text poisition - this ensure that character will not be divided
 *          at the end of row, the whole character will be depicted on the new row
 *
 * @param   uint8_t column
 * @param   uint8_t page
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdatePosition (uint8_t x, uint8_t p)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // check position
  // -------------------------------------------------------------------------------------
  if (x > END_COLUMN_ADDR) {
    // last page not reached
    // -----------------------------------------------------------------------------------
    if (p < END_PAGE_ADDR) {
      _indexCol = 0;                                              // update column
      _indexPage = _indexPage + 1;                                // update page
      status = SSD1306_SetPosition (_indexCol, _indexPage);       // update position
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
    }
    // last page reached
    // -----------------------------------------------------------------------------------
    if (p >= END_PAGE_ADDR) {
      return SSD1306_ERROR;                                       // return out of range
    }
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 * @param   enum font 
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar (char ch, enum E_Font font)
{
  uint8_t byte;
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint16_t i = 0;                                                 // counter
  uint8_t next_x;
  uint8_t next_p;
  uint8_t mask = 0x00;

  //  NORMAL FONT
  // -------------------------------------------------------------------------------------
  if ((font & 0x0f) == NORMAL) { 

    if (font & 0xf0) {
      mask = 0x80;
    }

    next_x = _indexCol + CHARS_COLS_LENGTH;
    next_p = _indexPage;

    // UPDATE / CHECK TEXT POSITION
    // -----------------------------------------------------------------------------------
    status = SSD1306_UpdatePosition (next_x, next_p);
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }

    // TWI START & SLAW
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);            // start & SLAW
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    // TWI control byte data stream
    // -----------------------------------------------------------------------------------
    status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);              // send data 0x40
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }

    while (i < CHARS_COLS_LENGTH) {
      byte = pgm_read_byte (&FONTS[ch-32][i]);
      status = TWI_MT_Send_Data (byte | mask);                    // send data col
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
      i++;                                                        // update counter
      _indexCol++;                                                // update global col
    }
  //  BOLD FONT
  // -------------------------------------------------------------------------------------
  } else if ((font & 0x0f) == BOLD) {

    if (font & 0xf0) {
      mask = 0x80;
    }

    next_x = _indexCol + 2*CHARS_COLS_LENGTH;
    next_p = _indexPage;

    // UPDATE / CHECK TEXT POSITION
    // -----------------------------------------------------------------------------------
    status = SSD1306_UpdatePosition (next_x, next_p);
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }

    // TWI START & SLAW
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);            // start & SLAW
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    // TWI control byte data stream
    // -----------------------------------------------------------------------------------
    status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);              // send data 0x40
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }

    while (i < CHARS_COLS_LENGTH) {
      byte = pgm_read_byte (&FONTS[ch-32][i]);
      status = TWI_MT_Send_Data (byte | mask);                    // send data col
      status = TWI_MT_Send_Data (byte | mask);                    // send data col
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
      i++;                                                        // update counter
      _indexCol = _indexCol + 2;                                  // update global col
    }
  } 
  //  send empty column to memory lcd
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (mask);                               // ONE empty column
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  _indexCol++;                                                    // update global col

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawCharBig (char ch)
{
  uint8_t byte;
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint16_t i = 0;                                                 // counter
  uint8_t next_x = _indexCol + 2*CHARS_COLS_LENGTH + 1;
  uint8_t next_p = _indexPage + 1;

  // UPDATE / CHECK TEXT POSITION
  // -------------------------------------------------------------------------------------
  status = SSD1306_UpdatePosition(next_x, next_p);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);                // send data 0x40
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  //  FONT 2x
  // -------------------------------------------------------------------------------------
  while (i < CHARS_COLS_LENGTH) {
    byte = pgm_read_byte (&FONTS[ch-32][i]);
    status = TWI_MT_Send_Data (byte);                             // send data col
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    i++;                                                          // update counter
    _indexCol++;                                                  // update global col
  }
  //  send empty column to memory lcd
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (CLEAR_COLOR);                        // ONE empty column
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  _indexCol++;                                                    // update global col

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char * string
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawString (char *str, enum E_Font font)
{
  uint8_t i = 0;                                                  // char counter

  // send characters of string
  // -------------------------------------------------------------------------------------
  while (str[i] != '\0') {
    SSD1306_DrawChar (str[i++], font);                            // send char
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    Draw pixel
 *
 * @param   uint8_t x -> 0 ... MAX_X
 * @param   uint8_t y -> 0 ... MAX_Y
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawPixel (uint8_t x, uint8_t y)
{
  uint8_t page = y >> 3;                                          // find page (y / 8)
  uint8_t pixel = 1 << (y - (page << 3));                         // which pixel (y % 8)
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  if ((x > MAX_X) || (y > MAX_Y)) {                               // if out of range
    return SSD1306_ERROR;                                         // error
  }

  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_COLUMN_ADDR);        // 0x21
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // start COLUMN
  status = SSD1306_Send_Command (x);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // end COLUMN
  status = SSD1306_Send_Command (x);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_PAGE_ADDR);          // 0x22
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // start PAGE
  status = SSD1306_Send_Command (page);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // end PAGE
  status = SSD1306_Send_Command (page);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_DATA);                       // send data 0xC0
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  //  PIXEL
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (pixel);                              // send pixel
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    Draw line horizontal
 *
 * @param   uint8_t
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLineHorz (uint8_t y, uint8_t x1, uint8_t x2)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint8_t i = 0;                                                  // counter for y dim
  uint8_t j = 0;                                                  // counter for x dim
  uint8_t page = y >> 3;                                          // page
  uint8_t pixel = 1 << (y - (page << 3));                         // which pixel
  uint8_t ram[RAM_Y_END][RAM_X_END];                              // buffer [3/7 x 127]

  // fill RAM buffer
  // -------------------------------------------------------------------------------------
  while (i < RAM_Y_END)
  {
    if (i == page) {                                              // for correspond page
      //memset (ram[i], pixel, RAM_X_END);                        // draw line
      while (j < x1) {                                            //
        ram[i][j++] = CLEAR_COLOR;                                // clear to x1
      }
      while (j < x2) {                                            //
        ram[i][j++] = pixel;                                      // set pixels to x2
      }
      while (j < RAM_X_END) {                                     //
        ram[i][j++] = CLEAR_COLOR;                                // clear to x end
      }
    } else {                                                      //
      memset (ram[i], CLEAR_COLOR, RAM_X_END);                    // clear whole page
    }
    i++;                                                          // next page
  }
  // TWI START & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (SSD1306_ADDR);              // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // COLUMN
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_COLUMN_ADDR);        // 0x21
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // start COLUMN
  status = SSD1306_Send_Command (START_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // end COLUMN
  status = SSD1306_Send_Command (END_COLUMN_ADDR);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // PAGE
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_SET_PAGE_ADDR);          // 0x22
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // start PAGE
  status = SSD1306_Send_Command (START_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // end PAGE
  status = SSD1306_Send_Command (END_PAGE_ADDR);
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);                // send data 0x40
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  //  PIXELS
  // -------------------------------------------------------------------------------------
  i = 0;
  j = 0;
  while (i < RAM_Y_END) {
    while (j < RAM_X_END) {
      status = TWI_MT_Send_Data (ram[i][j]);                      // send RAM buffer
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
      j++;                                                        // next column
    }
    j = 0;                                                        // null x counter
    i++;                                                          // next page
  }

  // TWI STOP
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success return
}

/**
 * @desc    Draw line by Bresenham algoritm
 *
 * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint8_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1
 * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLine (uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  int16_t D;                                                      // determinant
  int16_t delta_x, delta_y;                                       // deltas
  int16_t trace_x = 1, trace_y = 1;                               // steps

  if ((x1 > MAX_X) || (y1 > MAX_Y) ||
      (x2 > MAX_X) || (y2 > MAX_Y) ) {                            // out of range?
    return SSD1306_ERROR;                                         // error
  }

  if ((x1 > x2) || (y1 > y2) ) {                                  // start point first?
    return SSD1306_ERROR;                                         // error
  }

  delta_x = x2 - x1;                                              // delta x
  delta_y = y2 - y1;                                              // delta y

  //  Horizontal line
  // -------------------------------------------------------------------------------------
  if (delta_y == 0) {
    status = SSD1306_DrawLineHorz (y1, x1, x2);                   // draw horizontal line
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    return SSD1306_SUCCESS;                                       // success return
  }

  // condition for m < 1 (dy < dx)
  // -------------------------------------------------------------------------------------
  if (delta_y < delta_x) {                                        //
    D = (delta_y << 1) - delta_x;                                 // calculate determinant
    SSD1306_DrawPixel (x1, y1);                                   // draw first pixel
    while (x1 != x2) {                                            // check if x1 equal x2
      x1 += trace_x;                                              // update x1
      if (D >= 0) {                                               // positive?
        y1 += trace_y;                                            // update y1
        D -= 2*delta_x;                                           // update determinant
      }
      D += 2*delta_y;                                             // update deteminant
      SSD1306_DrawPixel (x1, y1);                                 // draw next pixel
    }
  // condition for m >= 1 (dy >= dx)
  // -------------------------------------------------------------------------------------
  } else {
    D = delta_y - (delta_x << 1);                                 // calculate determinant
    SSD1306_DrawPixel (x1, y1);                                   // draw first pixel
    while (y1 != y2) {                                            // check if y2 equal y1
      y1 += trace_y;                                              // update y1
      if (D <= 0) {                                               // positive?
        x1 += trace_x;                                            // update y1
        D += 2*delta_y;                                           // update determinant
      }
      D -= 2*delta_x;                                             // update deteminant
      SSD1306_DrawPixel (x1, y1);                                 // draw next pixel
    }
  }

  return SSD1306_SUCCESS;                                         // success return
}
