/** 
 * --------------------------------------------------------------------------------------+
 * @desc        Two Wire Interface / I2C Communication
 * --------------------------------------------------------------------------------------+
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @date        06.09.2020
 * @file        twi.c
 * @tested      AVR Atmega16, ATmega8, Atmega328
 *
 * @depend      twi.h
 * --------------------------------------------------------------------------------------+
 * @usage       Master Transmit Operation
 */
 
// include libraries
#include "twi.h"

/**
 * @desc    TWI init - initialize frequency
 *
 * @param   void
 *
 * @return  void
 */
void TWI_Init (void)
{
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // Calculation fclk:
  //
  // fclk = (fcpu)/(16+2*TWBR*4^Prescaler) m16
  // fclk = (fcpu)/(16+2*TWBR*Prescaler) m328p
  // -------------------------------------------------------------------------------------
  // Calculation TWBR:
  // 
  // TWBR = {(fcpu/fclk) - 16 } / (2*4^Prescaler)
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // @param1 value of TWBR (m16) 
  //  fclk = 400kHz; TWBR = 3
  //  fclk = 100kHz; TWBR = 20
  // @param1 value of TWBR (m328p)
  //  fclk = 400kHz; TWBR = 2
  // @param2 value of Prescaler = 1
  TWI_FREQ (2, 1);
}

/**
 * @desc    TWI MT Start
 *
 * @param   void
 *
 * @return  char
 */
char TWI_MT_Start (void)
{
  TWI_TWSR &= ~0xA8;                                              // null status flag

  TWI_START();
  TWI_WAIT_TILL_TWINT_IS_SET();

  if ((TWI_STATUS != TWI_START_ACK) && (TWI_STATUS != TWI_REP_START_ACK)) {
    return TWI_STATUS;
  }
 
  return SUCCESS;
}

/**
 * @desc    TWI Send address + write
 *
 * @param   char
 *
 * @return  char
 */
char TWI_MT_Send_SLAW(char address)
{
  TWI_TWDR = (address << 1);                                      // SLA+W

  TWI_ENABLE();
  TWI_WAIT_TILL_TWINT_IS_SET();

  if (TWI_STATUS != TWI_MT_SLAW_ACK) {
    return TWI_STATUS;
  }

  return SUCCESS;
}

/**
 * @desc    TWI Send data
 *
 * @param   char
 *
 * @return  char
 */
char TWI_MT_Send_Data(char data)
{
  TWI_TWDR = data;                                                // DATA

  TWI_ENABLE();
  TWI_WAIT_TILL_TWINT_IS_SET();

  if (TWI_STATUS != TWI_MT_DATA_ACK) {
    return TWI_STATUS;
  }
  return SUCCESS;
}

/**
 * @desc    TWI Send address + read
 *
 * @param   char
 *
 * @return  char
 */
char TWI_MR_Send_SLAR(char address)
{
  TWI_TWDR = (address << 1) | 0x01;                               // SLA+R

  TWI_ENABLE();
  TWI_WAIT_TILL_TWINT_IS_SET();

  if (TWI_STATUS != TWI_MR_SLAR_ACK) {
    return TWI_STATUS;
  }

  return SUCCESS;
}

/**
 * @desc    TWI stop
 *
 * @param   void
 *
 * @return  void
 */
void TWI_Stop(void)
{

  TWI_STOP();
  //TWI_WAIT_TILL_TWINT_IS_SET();
}
