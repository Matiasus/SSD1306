/** 
 * ---------------------------------------------------------------+ 
 * @desc        FONTs
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       07.10.2020
 * @file        font.h
 * @tested      AVR Atmega16
 *
 * @depend      
 * ---------------------------------------------------------------+
 */
#include <avr/pgmspace.h>

#ifndef __FONT_H__
#define __FONT_H__

  // Characters definition
  // -----------------------------------
  // number of columns for chars
  #define CHARS_COLS_LENGTH  5
  // @const Characters
  extern const uint8_t FONTS[][CHARS_COLS_LENGTH];

#endif
