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
 * @file        ssd1306.c
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

/** @array Init command */
const uint8_t INIT_SSD1306[] PROGMEM = {
  // number of initializers
  18,
  // 0xAE = Set Display OFF
  0, SSD1306_DISPLAY_OFF,
  // 0xA8
  1, SSD1306_SET_MUX_RATIO, 0x3F,
  // 0x20 = Set Memory Addressing Mode
  // -----------------------------------
  // 0x00 - Horizontal Addressing Mode
  // 0x01 - Vertical Addressing Mode
  // 0x02 - Page Addressing Mode (RESET)
  1, SSD1306_MEMORY_ADDR_MODE, 0x00,
  // 0x21 = Set Column Address
  // -----------------------------------
  // 0x00 - Start Column Address
  // 0xFF - End Column Address
  2, SSD1306_SET_COLUMN_ADDR, START_COLUMN_ADDR, END_COLUMN_ADDR,
  // 0x22 = Set Page Address
  // -----------------------------------
  // 0x00 - Start Column Address
  // 0x07 - End Column Address
  2, SSD1306_SET_PAGE_ADDR, START_PAGE_ADDR, END_PAGE_ADDR,
  // 0x40
  0, SSD1306_SET_START_LINE,
  // 0xD3
  1, SSD1306_DISPLAY_OFFSET, 0x00,
  // 0xA0 / remap 0xA1
  0, SSD1306_SEG_REMAP_OP,
  // 0xC0 / remap 0xC8
  0, SSD1306_COM_SCAN_DIR_OP,
  // 0xDA
  1, SSD1306_COM_PIN_CONF, 0x12,
  // 0x81
  1, SSD1306_SET_CONTRAST, 0x7F,
  // 0xA4
  0, SSD1306_DIS_ENT_DISP_ON,
  // 0xA6
  0, SSD1306_DIS_NORMAL,
  // 0xD5
  1, SSD1306_SET_OSC_FREQ, 0x80,
  // 0xD9, 1st Period = higher value less blinking
  1, SSD1306_SET_PRECHARGE, 0xc2,
  // Set V COMH Deselect, reset value 0x22 = 0,77xUcc
  1, SSD1306_VCOM_DESELECT, 0x20,
  // 0x8D
  1, SSD1306_SET_CHAR_REG, 0x14,
  // 0xAF = Set Display ON
  0, SSD1306_DISPLAY_ON
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
  // variables
  const uint8_t *commands = INIT_SSD1306;
  // number of commands
  unsigned short int no_of_commands = pgm_read_byte(commands++);
  // argument
  uint8_t no_of_arguments;
  // command
  uint8_t command;
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: Init
  // -------------------------
  TWI_Init ();

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // loop through commands
  while (no_of_commands) {

    // number of arguments
    no_of_arguments = pgm_read_byte (commands++);
    // command
    command = pgm_read_byte (commands++);

    // send command
    // -------------------------    
    status = SSD1306_Send_Command (command);
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }

    // send arguments
    // -------------------------
    while (no_of_arguments--) {
      // send command
      status = SSD1306_Send_Command (pgm_read_byte(commands++));
      // request - start TWI
      if (SSD1306_SUCCESS != status) {
        // error
        return status;
      }
    }
    // decrement
    no_of_commands--;
  }

  // TWI: Stop
  // -------------------------
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
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
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: start
  // -------------------------
  status = TWI_MT_Start ();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // TWI: send SLAW
  // -------------------------
  status = TWI_MT_Send_SLAW (address);
  // request - send SLAW
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
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
  // init status
  uint8_t status = INIT_STATUS;

  // send control byte
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_COMMAND);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = TWI_MT_Send_Data (command);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
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
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_DIS_NORMAL);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
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
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_DIS_INVERSE);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
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
  // init status
  uint8_t status = INIT_STATUS;
  // init i
  uint16_t i = 0;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request succesfull
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  //  send cache memory lcd
  // -------------------------
  while (i < CACHE_SIZE_MEM) {
    // send data
    status = TWI_MT_Send_Data (cacheMemLcd[i]);
    // request succesfull
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
    // increment
    i++;
  }

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
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
  // null cache memory lcd
  memset (cacheMemLcd, 0x00, CACHE_SIZE_MEM);
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
  // calculate counter
  _counter = x + (y << 7);
}

/**
 * @desc    SSD1306 Update text poisition - this ensure that character will not be divided at the end of row, the whole character will be depicted on the new row
 *
 * @param   void
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdTxtPosition (void) 
{
  // y / 8
  uint8_t y = _counter >> 7;
  // y % 8
  uint8_t x = _counter - (y << 7);
  // x + character length + 1
  uint8_t x_new = x + CHARS_COLS_LENGTH + 1;

  // check position
  if (x_new > END_COLUMN_ADDR) {
    // if more than allowable number of pages
    if (y > END_PAGE_ADDR) {
      // return out of range
      return SSD1306_ERROR;
    // if x reach the end but page in range
    } else if (y < (END_PAGE_ADDR-1)) {
      // update
      _counter = ((++y) << 7);
    }
  }
 
  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw character
 *
 * @param   char character
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar (char character)
{
  // variables
  uint8_t i = 0;

  // update text position
  // this ensure that character will not be divided at the end of row, the whole character will be depicted on the new row
  if (SSD1306_UpdTxtPosition () == SSD1306_ERROR) {
    // error
    return SSD1306_ERROR;
  }

  // loop through 5 bits
  while (i < CHARS_COLS_LENGTH) {
    // read byte 
    cacheMemLcd[_counter++] = pgm_read_byte(&FONTS[character-32][i++]);
  }

  // update position
  _counter++;

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char * string
 *
 * @return  void
 */
void SSD1306_DrawString (char *str)
{
  // init
  int i = 0;
  // loop through character of string
  while (str[i] != '\0') {
    // draw string
    SSD1306_DrawChar (str[i++]);
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

  // if out of range
  if ((x > MAX_X) && (y > MAX_Y)) {
    // out of range
    return SSD1306_ERROR;
  }
  // find page (y / 8)
  page = y >> 3;
  // which pixel (y % 8)
  pixel = 1 << (y - (page << 3));
  // update counter
  _counter = x + (page << 7);
  // save pixel
  cacheMemLcd[_counter++] |= pixel;

  // success
  return SSD1306_SUCCESS;
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
  // determinant
  int16_t D;
  // deltas
  int16_t delta_x, delta_y;
  // steps
  int16_t trace_x = 1, trace_y = 1;

  // delta x
  delta_x = x2 - x1;
  // delta y
  delta_y = y2 - y1;

  // check if x2 > x1
  if (delta_x < 0) {
    // negate delta x
    delta_x = -delta_x;
    // negate step x
    trace_x = -trace_x;
  }

  // check if y2 > y1
  if (delta_y < 0) {
    // negate detla y
    delta_y = -delta_y;
    // negate step y
    trace_y = -trace_y;
  }

  // Bresenham condition for m < 1 (dy < dx)
  if (delta_y < delta_x) {
    // calculate determinant
    D = (delta_y << 1) - delta_x;
    // draw first pixel
    SSD1306_DrawPixel (x1, y1);
    // check if x1 equal x2
    while (x1 != x2) {
      // update x1
      x1 += trace_x;
      // check if determinant is positive
      if (D >= 0) {
        // update y1
        y1 += trace_y;
        // update determinant
        D -= 2*delta_x;    
      }
      // update deteminant
      D += 2*delta_y;
      // draw next pixel
      SSD1306_DrawPixel (x1, y1);
    }
  // for m > 1 (dy > dx)    
  } else {
    // calculate determinant
    D = delta_y - (delta_x << 1);
    // draw first pixel
    SSD1306_DrawPixel (x1, y1);
    // check if y2 equal y1
    while (y1 != y2) {
      // update y1
      y1 += trace_y;
      // check if determinant is positive
      if (D <= 0) {
        // update y1
        x1 += trace_x;
        // update determinant
        D += 2*delta_y;    
      }
      // update deteminant
      D -= 2*delta_x;
      // draw next pixel
      SSD1306_DrawPixel (x1, y1);
    }
  }
  // success return
  return SSD1306_SUCCESS;
}
