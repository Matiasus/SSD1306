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
 * @depend      font.h, twi.h
 * ---------------------------------------------------------------+
 */
#include <avr/pgmspace.h>
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

// @var global -  set area
unsigned int set_area = (END_PAGE_ADDR - START_PAGE_ADDR + 1) * (END_COLUMN_ADDR - START_COLUMN_ADDR + 1);
// @var global - cache index column
unsigned short int indexCol = START_COLUMN_ADDR;
// @var global - cache index page
unsigned short int indexPage = START_PAGE_ADDR;

/**
 * @desc    SSD1306 Init
 *
 * @param   void
 *
 * @return  char
 */
char SSD1306_Init(void)
{ 
  // variables
  const uint8_t *commands = INIT_SSD1306;
  // number of commands
  unsigned short int no_of_commands = pgm_read_byte(commands++);
  // argument
  char no_of_arguments;
  // command
  char command;
  // init status
  char status = INIT_STATUS;

  // TWI: Init
  // -------------------------
  TWI_Init();

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // loop throuh commands
  while (no_of_commands) {

    // number of arguments
    no_of_arguments = pgm_read_byte(commands++);
    // command
    command = pgm_read_byte(commands++);

    // send command
    // -------------------------    
    status = SSD1306_Send_Command(command);
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }

    // send arguments
    // -------------------------
    while (no_of_arguments--) {
      // send command
      status = SSD1306_Send_Command(pgm_read_byte(commands++));
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
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Send Start and SLAW request
 *
 * @param   void
 *
 * @return  char
 */
char SSD1306_Send_StartAndSLAW(void)
{
  // init status
  char status = INIT_STATUS;

  // TWI: start
  // -------------------------
  status = TWI_MT_Start();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // TWI: send SLAW
  // -------------------------
  status = TWI_MT_Send_SLAW(SSD1306_ADDRESS);
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
 * @param   char
 *
 * @return  char
 */
char SSD1306_Send_Command(char command)
{
  // init status
  char status = INIT_STATUS;

  // send control byte
  // -------------------------    
  status = TWI_MT_Send_Data(SSD1306_COMMAND);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = TWI_MT_Send_Data(command);
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
 * @param   void
 *
 * @return  char
 */
char SSD1306_NormalScreen(void)
{
  // init status
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command(SSD1306_DIS_NORMAL);
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
 * @param   void
 *
 * @return  char
 */
char SSD1306_InverseScreen(void)
{
  // init status
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command(SSD1306_DIS_INVERSE);
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
 * @param   void
 *
 * @return  char
 */
char SSD1306_UpdateScreen(void)
{
  // init status
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // send command
  // -------------------------    
  status = SSD1306_Send_Command(SSD1306_DISPLAY_ON);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Send Command
 *
 * @param   void
 *
 * @return  char
 */
char SSD1306_ClearScreen(void)
{
  // init status
  char status = INIT_STATUS;
  short int i = 0;

  // update - null col, increment page
  status = SSD1306_SetPosition(0, 0);
  // request
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // erase whole area
  // -------------------------
  while (i < set_area) {
    // send null data
    status = TWI_MT_Send_Data(0x00);
    // request - start TWI
    if (SSD1306_SUCCESS != status) {
      // error
      return status;
    }
    // increment
    i++;
  }


  // stop TWI
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;  
}

/**
 * @desc    SSD1306 Set Poisition
 *
 * @param   char
 * @param   char
 *
 * @return  char
 */
char SSD1306_SetPosition(char x, char y) 
{
  // variables
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // SET COLUMN address 
  // ***************************************************
  // set column addr
  // -------------------------    
  status = SSD1306_Send_Command(SSD1306_SET_COLUMN_ADDR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start COLUMN
  // -------------------------    
  status = SSD1306_Send_Command(x);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end COLUMN
  // -------------------------    
  status = SSD1306_Send_Command(END_COLUMN_ADDR);
  // request - start TWI
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
  status = SSD1306_Send_Command(SSD1306_SET_PAGE_ADDR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // start PAGE
  // -------------------------    
  status = SSD1306_Send_Command(y);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // end PAGE
  // -------------------------    
  status = SSD1306_Send_Command(END_PAGE_ADDR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // update column index
  indexPage = y;

  // stop TWI
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Check Text Poisition
 *
 * @param   char
 * @param   char
 *
 * @return  char
 */
char SSD1306_UpdTxtPosition(void) 
{
  // init status
  char status = INIT_STATUS;
  // check end column position
  unsigned short int x = indexCol+CHARS_COLS_LENGTH+1;
  // check position
  if ((x > END_COLUMN_ADDR) && (indexPage > (END_PAGE_ADDR-1))) {
    // return out of range
    return SSD1306_ERROR;
  // if x out reach end but page in range
  } else if ((x > END_COLUMN_ADDR) && (indexPage < END_PAGE_ADDR)) {
    // update - column
    indexCol = 0;
    // update - page
    indexPage = indexPage+1;
    // update - null col, increment page
    status = SSD1306_SetPosition(indexCol, indexPage);
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
 * @desc    Draw character
 *
 * @param   char
 *
 * @return  void
 */
char SSD1306_DrawChar(char character)
{
  // variables
  uint8_t idxCol = 0;
  // init status
  char status = INIT_STATUS;

  // update text position
  if (SSD1306_UpdTxtPosition() != SSD1306_SUCCESS) {
    // error
    return SSD1306_ERROR;
  }

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // loop through 5 bits
  while (idxCol < CHARS_COLS_LENGTH) {
    // send control byte data
    // -------------------------    
    status = TWI_MT_Send_Data(pgm_read_byte(&FONTS[character-32][idxCol]));
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
  status = TWI_MT_Send_Data(CLEAR_COLOR);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // increment global index col
  indexCol = indexCol + CHARS_COLS_LENGTH + 1;

  // stop TWI
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Send 1 Byte of data
 *
 * @param   char
 *
 * @return  void
 */
char SSD1306_SendByte(char data)
{
  // init status
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }
  // send byte of data
  // -------------------------    
  status = TWI_MT_Send_Data(data);
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // increment global index col
  indexCol = indexCol + 1;

  // stop TWI
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    Send Same Bytes
 *
 * @param   char
 * @param   char
 *
 * @return  void
 */
char SSD1306_SendBytes(char data, char length)
{
  // index
  unsigned short int i = 0;
  // init status
  char status = INIT_STATUS;

  // TWI: start & SLAW
  // -------------------------
  status = SSD1306_Send_StartAndSLAW();
  // request - start TWI
  if (SSD1306_SUCCESS != status) {
    // error
    return status;
  }

  // control byte data stream
  // -------------------------    
  status = TWI_MT_Send_Data(SSD1306_DATA_STREAM);
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
      status = TWI_MT_Send_Data(data);
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
  TWI_Stop();

  // success
  return SSD1306_SUCCESS;
}

/**
 * @desc    SSD1306 Draw String
 *
 * @param   char *
 *
 * @return  void
 */
char SSD1306_DrawString(char *str)
{
  // init status
  char status = INIT_STATUS;
  // init
  int i = 0;

  // loop through character of string
  while (str[i] != '\0') {
    // draw string
    status = SSD1306_DrawChar(str[i++]);
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
 * @desc    Draw horizontal line
 *
 * @param   char
 * @param   char
 * @param   char
 *
 * @return  void
 */
char SSD1306_DrawLineHorizontal(char x, char y, char len)
{
  char page = 0;
  char pixel = 0;

  // if out of range
  if ((x > MAX_X) && (y > MAX_Y)) {
    // out of range
    return SSD1306_ERROR;
  }
  // find page
  page = y / 8;
  // which pixel
  pixel |= 1 << (y % 8);

  // send position
  SSD1306_SetPosition(x, page);
  // draw pixel
  SSD1306_SendBytes(pixel, len);

  // success
  return SSD1306_SUCCESS;  
}

