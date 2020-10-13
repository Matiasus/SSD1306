/** 
 * ---------------------------------------------------+ 
 * @desc        Two Wire Interface / I2C Communication
 * ---------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.09.2020
 * @file        twi.c
 * @tested      AVR Atmega16
 * ---------------------------------------------------
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
void TWI_Init(void)
{
  // +++++++++++++++++++++++++++++++++++++++++++++
  // Calculation fclk:
  //
  // fclk = (fcpu)/(16+2*TWBR*4^Prescaler)
  // --------------------------------------------- 
  // Calculation TWBR:
  // 
  // TWBR = {(fcpu/fclk) - 16 } / (2*4^Prescaler)
  // +++++++++++++++++++++++++++++++++++++++++++++
  // @param1 value of TWBR, 
  //    fclk = 400 kHz; TWBR = 3
  //    fclk = 100 kHz; TWBR = 20
  // @param2 value of Prescaler = 1
  TWI_FREQ(20, 1);
}

/**
 * @desc    TWI MT Start
 *
 * @param   void
 *
 * @return  char
 */
char TWI_MT_Start(void)
{
  // null status flag
  TWI_TWSR &= ~0xA8;
  // START
  // ----------------------------------------------
  // request for bus
  TWI_START();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();
  // test if start or repeated start acknowledged
  if ((TWI_STATUS != TWI_START_ACK) && (TWI_STATUS != TWI_REP_START_ACK)) {
    // return status
    return TWI_STATUS;
  }
  // success
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
  // SLA+W
  // ----------------------------------------------
  TWI_TWDR = (address << 1);
  // enable
  TWI_ENABLE();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // find
  if (TWI_STATUS != TWI_MT_SLAW_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return found device address
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
  // DATA
  // ----------------------------------------------
  TWI_TWDR = data;
  // enable
  TWI_ENABLE();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // find
  if (TWI_STATUS != TWI_MT_DATA_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return found device address
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
  // SLA+R
  // ----------------------------------------------
  TWI_TWDR = (address << 1) | 0x01;
  // enable
  TWI_ENABLE();
  // wait till flag set
  TWI_WAIT_TILL_TWINT_IS_SET();

  // find
  if (TWI_STATUS != TWI_MR_SLAR_ACK) {
    // return status
    return TWI_STATUS;
  }
  // return found device address
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
  // End TWI
  // -------------------------------------------------
  // send stop sequence
  TWI_STOP();
  // wait for TWINT flag is set
//  TWI_WAIT_TILL_TWINT_IS_SET();
}
