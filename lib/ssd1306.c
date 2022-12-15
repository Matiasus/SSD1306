/**
 * --------------------------------------------------------------------------------------+
 * @desc        SSD1306 OLED Driver
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.10.2020
 * @update      15.12.2022
 * @file        ssd1306.c
 * @version     3.0.0
 * @tested      AVR Atmega328p
 *
 * @depend      ssd1306.h
 * --------------------------------------------------------------------------------------+
 * @descr       Version 1.0.0 -> applicable for 1 display
 *              Version 2.0.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0.0 -> simplified alphanumeric version for 1 display
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
  1, SSD1306_SET_MUX_RATIO, 63,                                   // 0xA8 - 64MUX for 128 x 64 version
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
  1, SSD1306_COM_PIN_CONF, 0x12,                                  // 0xDA, 0x12 - Disable COM Left/Right remap, Alternative COM pin configuration
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
 * @param   uint8_t page -> 0 ... 3 or 7
 * @param   uint8_t page -> 0 ... 3 or 7
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
  uint8_t i = 0;                                                  // counter
  uint8_t next_x;
  uint8_t next_p;
  uint8_t mask = 0x00;

  //  NORMAL FONT
  // -------------------------------------------------------------------------------------
  if ((font & 0x0f) == NORMAL) { 

    if (font & 0xf0) {                                            // underline?
      mask = 0x80;                                                // set underline mask
    }

    next_x = _indexCol + CHARS_COLS_LENGTH;                       // next column
    next_p = _indexPage;                                          // next page

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
      byte = pgm_read_byte (&FONTS[ch-32][i++]);
      status = TWI_MT_Send_Data (byte | mask);                    // send data col
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
      _indexCol++;                                                // update global col
    }
  //  BOLD FONT
  // -------------------------------------------------------------------------------------
  } else if ((font & 0x0f) == BOLD) {

    if (font & 0xf0) {                                            // underline?
      mask = 0x80;                                                // set underline mask
    }

    next_x = _indexCol + (CHARS_COLS_LENGTH << 1);                // next column
    next_p = _indexPage;                                          // next page

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
      byte = pgm_read_byte (&FONTS[ch-32][i++]);
      status = TWI_MT_Send_Data (byte | mask);                    // send data 1st col
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
      status = TWI_MT_Send_Data (byte | mask);                    // send data 2nd col
      if (SSD1306_SUCCESS != status) {                            // check status
        return status;                                            // error
      }
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
