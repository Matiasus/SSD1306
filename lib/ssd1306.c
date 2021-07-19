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
 * @tested      AVR Atmega328
 *
 * @depend      font.h, twi.h
 * ---------------------------------------------------------------+
 * @descr       Version 1.0 -> applicable for 1 display
 *              Version 2.0 -> applicable for more than 1 display
 * ---------------------------------------------------------------+
 * @usage       Basic Setup for OLED Display
 */
 
#include <avr/pgmspace.h>
#include <string.h>
#include "font.h"
#include "twi.h"
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
  // -----------------------------------
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
  // -----------------------------------
  0, SSD1306_DISPLAY_ON
};

// @var array Chache memory Lcd 8 * 128 = 1024
static char cacheMemLcd[CACHE_SIZE_MEM];

// @var global -  set area
unsigned int set_area = (END_PAGE_ADDR - START_PAGE_ADDR + 1) * (END_COLUMN_ADDR - START_COLUMN_ADDR + 1);
// @var global - cache index column
unsigned short int indexCol = START_COLUMN_ADDR;
// @var global - cache index page
unsigned short int indexPage = START_PAGE_ADDR;

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

  // loop throuh commands
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
 * @param   uint8_t
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
 * @desc    SSD1306 Normal Colors
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
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_DIS_NORMAL);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Inverse Colors
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
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_DIS_INVERSE);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Update Screen On
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
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // erase whole area
  // -------------------------
  while (i < CACHE_SIZE_MEM) {
    // send null data
    status = TWI_MT_Send_Data (cacheMemLcd[i]);
    // request - start TWI
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
 * @desc    SSD1306 Send Command
 *
 * @param   uint8_t address
 *
 * @return  void
 */
void SSD1306_ClearScreen (uint8_t address)
{
  // init status
  uint8_t status = INIT_STATUS;
  //short int i = 0;

  // null cache memory lcd
  memset (cacheMemLcd, 0x00, CACHE_SIZE_MEM);
/*
  // update - null col, increment page
  status = SSD1306_SetPosition (address, 0, 0);
  // request
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // erase whole area
  // -------------------------
  while (i < set_area) {
    // send null data
    status = TWI_MT_Send_Data (0x00);
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
    // increment
    i++;
  }

  // stop TWI
  TWI_Stop ();
*/
  // success
  return SSD1306_SUCCESS;  
}

/**
 * @desc    SSD1306 Set Poisition
 *
 * @param   uint8_t address
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  uint8_t
 */
uint8_t SSD1306_SetPosition (uint8_t address, uint8_t x, uint8_t y) 
{
  // variables
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // SET COLUMN address 
  // ***************************************************
  // set column addr
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_SET_COLUMN_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start COLUMN
  // -------------------------    
  status = SSD1306_Send_Command (x);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end COLUMN
  // -------------------------    
  status = SSD1306_Send_Command (END_COLUMN_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // update column index
  indexCol = x;

  // SET PAGE address 
  // ***************************************************
  // set page addr
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_SET_PAGE_ADDR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start PAGE
  // -------------------------    
  status = SSD1306_Send_Command (y);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end PAGE
  // -------------------------    
  status = SSD1306_Send_Command (END_PAGE_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // update column index
  indexPage = y;

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Check Text Poisition
 *
 * @param   uint8_t address
 *
 * @return  uint8_t
 */
uint8_t SSD1306_UpdTxtPosition (uint8_t address) 
{
  // init status
  uint8_t status = INIT_STATUS;
  // check end column position
  unsigned short int x = indexCol + CHARS_COLS_LENGTH + 1;
  // check position
  if ((x > END_COLUMN_ADDR) && (indexPage > (END_PAGE_ADDR - 1))) {
    // return out of range
    return SSD1306_ERROR;
  // if x out reach end but page in range
  } else if ((x > END_COLUMN_ADDR) && (indexPage < END_PAGE_ADDR)) {
    // update - column
    indexCol = 0;
    // update - page
    indexPage = indexPage + 1;
    // update - null col, increment page
    status = SSD1306_SetPosition (address, indexCol, indexPage);
    // request
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
  } 
  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Send 1 Byte of data
 *
 * @param   uint8_t address
 * @param   uint8_t data
 *
 * @return  void
 */
uint8_t SSD1306_SendByte (uint8_t address, uint8_t data)
{
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // send byte of data
  // -------------------------    
  status = TWI_MT_Send_Data (data);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // increment global index col
  indexCol = indexCol + 1;

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Send same bytes
 *
 * @param   uint8_t address
 * @param   uint8_t data
 * @param   uint8_t length
 *
 * @return  void
 */
uint8_t SSD1306_SendSameBytes (uint8_t address, uint8_t data, uint8_t length)
{
  // index
  unsigned short int i = 0;
  // init status
  uint8_t status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // loop through data
  while (i++ < length) {
    // draw line only within 1 page
    if (indexCol < MAX_X) {
      // send byte of data
      // -------------------------    
      status = TWI_MT_Send_Data (data);
      // request - start TWI
      if (SSD1306_SUCCESS != status) {
        // error
        return status;
      }
      // increment global index col
      indexCol = indexCol + 1;
    } else {
      // end loop
      break;
    }
  }

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Draw character
 *
 * @param   uint8_t address
 * @param   char character
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawChar (uint8_t address, char character)
{
  // variables
  uint8_t idxCol = 0;
  // init status
  uint8_t status = INIT_STATUS;

  // update text position
  if (SSD1306_UpdTxtPosition (address) != SSD1306_SUCCESS) {
    // error
    return SSD1306_ERROR;
  }

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // loop through 5 bits
  while (idxCol < CHARS_COLS_LENGTH) {
    // send control byte data
    // -------------------------    
    status = TWI_MT_Send_Data (pgm_read_byte(&FONTS[character-32][idxCol]));
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
    // increment
    idxCol++;
  }

  // empty column
  // -------------------------    
  status = TWI_MT_Send_Data (CLEAR_COLOR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // increment global index col
  indexCol = indexCol + CHARS_COLS_LENGTH + 1;

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   uint8_t address
 * @param   char * string
 *
 * @return  void
 */
uint8_t SSD1306_DrawString (uint8_t address, char *str)
{
  // init status
  uint8_t status = INIT_STATUS;
  // init
  int i = 0;

  // loop through character of string
  while (str[i] != '\0') {
    // draw string
    status = SSD1306_DrawChar (address, str[i++]);
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Draw pixel
 *
 * @param   uint8_t
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
uint8_t SSD1306_DrawPixel (uint8_t address, uint8_t x, uint8_t y)
{
  // variables
  uint8_t status = INIT_STATUS;
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

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW (address);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // SET COLUMN address 
  // ***************************************************
  // set column addr
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_SET_COLUMN_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start COLUMN
  // -------------------------    
  status = SSD1306_Send_Command (x);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end COLUMN
  // -------------------------    
  status = SSD1306_Send_Command (END_COLUMN_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // update column index
  indexCol = x;

  // SET PAGE address 
  // ***************************************************
  // set page addr
  // -------------------------    
  status = SSD1306_Send_Command (SSD1306_SET_PAGE_ADDR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start PAGE
  // -------------------------    
  status = SSD1306_Send_Command (page);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end PAGE
  // -------------------------    
  status = SSD1306_Send_Command (END_PAGE_ADDR);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // update column index
  indexPage = y;

  // ------------------------------------------------------
  // control byte data stream
  status = TWI_MT_Send_Data (SSD1306_DATA_STREAM);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // send byte of data
  status = TWI_MT_Send_Data (pixel);
  // request success
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // increment global index col
  indexCol = indexCol + 1;
  // ------------------------------------------------------

  // stop TWI
  TWI_Stop ();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Draw line by Bresenham algoritm
 * @surce   https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 *  
 * @param   uint8_t address
 * @param   uint8_t x start position / 0 <= cols <= MAX_X-1
 * @param   uint8_t x end position   / 0 <= cols <= MAX_X-1
 * @param   uint8_t y start position / 0 <= rows <= MAX_Y-1 
 * @param   uint8_t y end position   / 0 <= rows <= MAX_Y-1
 *
 * @return  uint8_t
 */
uint8_t SSD1306_DrawLine (uint8_t address, uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2)
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
    SSD1306_DrawPixel (address, x1, y1);
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
      SSD1306_DrawPixel (address, x1, y1);
    }
  // for m > 1 (dy > dx)    
  } else {
    // calculate determinant
    D = delta_y - (delta_x << 1);
    // draw first pixel
    SSD1306_DrawPixel (address, x1, y1);
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
      SSD1306_DrawPixel (address, x1, y1);
    }
  }
  // success return
  return SSD1306_SUCCESS;
}

/**
 * @desc    Draw horizontal line
 *
 * @param   uint8_t address
 * @param   uint8_t
 * @param   uint8_t
 * @param   uint8_t
 *
 * @return  void
 */
uint8_t SSD1306_DrawLineHorizontal (uint8_t address, uint8_t x, uint8_t y, uint8_t len)
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
  pixel |= 1 << (y - (page << 3));

  // send position
  SSD1306_SetPosition (address, x, page);
  // draw pixel
  SSD1306_SendSameBytes (address, pixel, len);

  // success
  return SSD1306_SUCCESS;  
}

