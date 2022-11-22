/**
 * ---------------------------------------------------------------+
 * @desc        SSD1306 OLED Driver
 * ---------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.10.2020
 * @update      21.11.2022
 * @file        ssd1306.c
 * @version     3.0
 * @tested      AVR Atmega328
 *
 * @depend      font.h, twi.h
 * ---------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> rebuild to 'cacheMemLcd' array
 *              Version 3.0 -> remove 'cacheMemLcd' approach
 * ---------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */

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
  1, SSD1306_SET_MUX_RATIO, 0x3F,                                 // 0xA8
  1, SSD1306_MEMORY_ADDR_MODE, 0x00,                              // 0x20 = Set Memory Addressing Mode
                                                                  // 0x00 - Horizontal Addressing Mode
                                                                  // 0x01 - Vertical Addressing Mode
                                                                  // 0x02 - Page Addressing Mode (RESET)
  2, SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR, // 0x21 = Set Column Address
  2, SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,       // 0x22 = Set Page Address
  0, SSD1306_SET_START_LINE,                                      // 0x40
  1, SSD1306_DISPLAY_OFFSET, 0x00,                                // 0xD3
  0, SSD1306_SEG_REMAP_OP,                                        // 0xA0 / remap 0xA1
  0, SSD1306_COM_SCAN_DIR_OP,                                     // 0xC0 / remap 0xC8
  1, SSD1306_COM_PIN_CONF, 0x12,                                  // 0xDA
  1, SSD1306_SET_CONTRAST, 0x7F,                                  // 0x81
  0, SSD1306_DIS_ENT_DISP_ON,                                     // 0xA4
  0, SSD1306_DIS_NORMAL,                                          // 0xA6
  1, SSD1306_SET_OSC_FREQ, 0x80,                                  // 0xD5
  1, SSD1306_SET_PRECHARGE, 0xc2,                                 // 0xD9, 1st Period = higher value less blinking
  1, SSD1306_VCOM_DESELECT, 0x20,                                 // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  1, SSD1306_SET_CHAR_REG, 0x14,                                  // 0x8D
  0, SSD1306_DISPLAY_ON                                           // 0xAF = Set Display ON
};

// @var array Chache memory Lcd 8 * 128 = 1024
static char cacheMemLcd[CACHE_SIZE_MEM];

/**
 * @desc    SSD1306 Init
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_Init (uint8_t address)
{
  const uint8_t *commands = INIT_SSD1306;                         // variables
  unsigned short int no_of_commands = pgm_read_byte(commands++);  // number of commands
  uint8_t no_of_arguments;                                        // number od arguments
  uint8_t command;                                                // command
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI INIT
  // -------------------------------------------------------------------------------------
  TWI_Init ();

  // TWI Start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (address);                   // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // MAIN LOOP
  // -------------------------------------------------------------------------------------
  while (no_of_commands) {                                        // commands loop
    no_of_arguments = pgm_read_byte (commands++);                 // number of arguments
    command = pgm_read_byte (commands++);                         // command
    // SEND COMMAND
    // -----------------------------------------------------------------------------------
    status = SSD1306_Send_Command (command);                      // send command
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    // SEND ARGUMENTS
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
  // TWI start & SLAW
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
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_NormalScreen (uint8_t address)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF

  // TWI start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (address);                   // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_DIS_NORMAL);             // command 0xA6
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Inverse colors
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_InverseScreen (uint8_t address)
{
  uint8_t status = INIT_STATUS;

  // TWI start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (address);                   // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send command
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_Command (SSD1306_DIS_INVERSE);            // command 0xA7
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Update screen
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdateScreen (uint8_t address)
{
  uint8_t status = INIT_STATUS;                                   // TWI init status 0xFF
  uint16_t i = 0;                                                 // counter
  
  // TWI start & SLAW
  // -------------------------------------------------------------------------------------
  status = SSD1306_Send_StartAndSLAW (address);                   // start & SLAW
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI control byte data stream
  // -------------------------------------------------------------------------------------
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);                // send data 0x40
  if (SSD1306_SUCCESS != status) {                                // check status
    return status;                                                // error
  }
  // TWI send cache memory lcd
  // -------------------------------------------------------------------------------------
  while (i < CACHE_SIZE_MEM) {
    status = TWI_MT_Send_Data (cacheMemLcd[i]);                   // send data
    if (SSD1306_SUCCESS != status) {                              // check status
      return status;                                              // error
    }
    i++;                                                          // next value
  }
  // TWI stop
  // -------------------------------------------------------------------------------------
  TWI_Stop ();

  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Clear screen
 *
 * @param   void
 *
 * @return  void
 */
void SSD1306_ClearScreen (void)
{
  memset (cacheMemLcd, 0x00, CACHE_SIZE_MEM);                     // null cache memory lcd
}

/**
 * @desc    SSD1306 Set position
 *
 * @param   uint8_t column -> 0 ... 127
 * @param   uint8_t page -> 0 ... 7
 *
 * @return  void
 */
void SSD1306_SetPosition (uint8_t x, uint8_t y)
{
  _counter = x + (y << 7);                                        // calculate counter
}

/**
 * @desc    SSD1306 Update text poisition - this ensure that character will not be
 *          divided at the end of row, the whole character will be depicted on the new row
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdTxtPosition (void)
{
  uint8_t y = _counter >> 7;                                      // y / 8
  uint8_t x = _counter - (y << 7);                                // y % 8
  uint8_t x_new = x + CHARS_COLS_LENGTH + 1;                      // x + character length + 1
  
  if (x_new > END_COLUMN_ADDR) {                                  // check position
    if (y > END_PAGE_ADDR) {                                      // if more than allowable number of pages
      return SSD1306_ERROR;                                       // return out of range
    } else if (y < (END_PAGE_ADDR-1)) {                           // if x reach the end but page in range
      _counter = ((++y) << 7);                                    // update
    }
  }
  
  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar (char ch)
{
  uint8_t i = 0;

  // update text position
  // this ensure that character will not be divided at the end of row, the whole character will be depicted on the new row
  if (SSD1306_UpdTxtPosition () == SSD1306_ERROR) {
    return SSD1306_ERROR;                                         // error
  }
  while (i < CHARS_COLS_LENGTH) {                                 // loop through 5 bits
    cacheMemLcd[_counter++] = pgm_read_byte(&FONTS[ch-32][i++]);  // read byte
  }
  _counter++;                                                     // update position
  
  return SSD1306_SUCCESS;                                         // success
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char * string
 *
 * @return  uint8_t
 */
void SSD1306_DrawString (char *str)
{
  int i = 0;                                                      // init
  
  while (str[i] != '\0') {                                        // loop through character of string
    SSD1306_DrawChar (str[i++]);                                  // draw string
  }
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
  uint8_t page = 0;
  uint8_t pixel = 0;

  if ((x > MAX_X) && (y > MAX_Y)) {                               // if out of range
    return SSD1306_ERROR;                                         // out of range
  }
  page = y >> 3;                                                  // find page (y / 8)
  pixel = 1 << (y - (page << 3));                                 // which pixel (y % 8)
  _counter = x + (page << 7);                                     // update counter
  cacheMemLcd[_counter++] |= pixel;                               // save pixel
  
  return SSD1306_SUCCESS;                                         // success
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
  int16_t D;                                                      // determinant
  int16_t delta_x, delta_y;                                       // deltas
  int16_t trace_x = 1, trace_y = 1;                               // steps

  delta_x = x2 - x1;                                              // delta x
  delta_y = y2 - y1;                                              // delta y

  if (delta_x < 0) {                                              // check if x2 > x1
    delta_x = -delta_x;                                           // negate delta x
    trace_x = -trace_x;                                           // negate step x
  }

  if (delta_y < 0) {                                              // check if y2 > y1
    delta_y = -delta_y;                                           // negate delta y
    trace_y = -trace_y;                                           // negate step y
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
