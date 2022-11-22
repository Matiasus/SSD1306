/** 
 * ---------------------------------------------------+ 
 * @desc        Two Wire Interface / I2C Communication
 * ---------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       06.09.2020
 * @file        twi.h
 * @tested      AVR Atmega16
 * ---------------------------------------------------
 */

#include <stdio.h>
#include <avr/io.h>

#ifndef __TWI_H__
#define __TWI_H__

  // define register for TWI communication
  #if defined(__AVR_ATmega16__) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega328P__)
    #define TWI_TWAR TWAR // TWI (Slave) Address Register
    #define TWI_TWBR TWBR // TWI Bit Rate Register
    #define TWI_TWDR TWDR // TWI Data Register
    #define TWI_TWCR TWCR // TWI Control Register
    #define TWI_TWSR TWSR // TWI Status Register
  #endif

  // TWI CLK frequency
  //  @param TWBR
  //  @param Prescaler
  //    TWPS1 TWPS0  - PRESCALER
  //      0     0    -     1
  //      0     1    -     4
  //      1     0    -    16
  //      1     1    -    64
  #define TWI_FREQ(BIT_RATE, PRESCALER) { TWI_TWBR = BIT_RATE; TWI_TWSR |= (TWI_TWSR & 0x03) | PRESCALER; }

  // TWI start condition
  // (1 <<  TWEN) - TWI Enable
  // (1 << TWINT) - TWI Interrupt Flag - must be cleared by set
  // (1 << TWSTA) - TWI Start
  #define TWI_START() { TWI_TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTA); }

  // TWI enable
  // (1 <<  TWEN) - TWI Enable
  // (1 << TWINT) - TWI Interrupt Flag - must be cleared by set
  #define TWI_ENABLE() { TWI_TWCR = (1 << TWEN) | (1 << TWINT); }

  // TWI stop condition
  // (1 <<  TWEN) - TWI Enable
  // (1 << TWINT) - TWI Interrupt Flag - must be cleared by set
  // (1 << TWSTO) - TWI Stop
  #define TWI_STOP() { TWI_TWCR = (1 << TWEN) | (1 << TWINT) | (1 << TWSTO); }

  // TWI test if TWINT Flag is set
  #define TWI_WAIT_TILL_TWINT_IS_SET() { while (!(TWI_TWCR & (1 << TWINT))); }

  // TWI status mask
  #define TWI_STATUS (TWI_TWSR & 0xF8)
 
  // success return value
  #define SUCCESS 0
  // success return value
  #define ERROR 1

  // ++++++++++++++++++++++++++++++++++++++++++
  //
  //        M A S T E R   M O D E
  //
  // ++++++++++++++++++++++++++++++++++++++++++  
  // Master Mode - Transmitter / Receiver
  #define TWI_START_ACK         0x08  // A START condition has been transmitted
  #define TWI_REP_START_ACK     0x10  // A repeated START condition has been transmitted
  #define TWI_FLAG_ARB_LOST     0x38  // Arbitration lost in SLA+W or NOT ACK bit
  // Master Transmitter Mode
  #define TWI_MT_SLAW_ACK       0x18  // SLA+W has been transmitted; ACK has been received
  #define TWI_MT_SLAW_NACK      0x20  // SLA+W has been transmitted; NOT ACK has been received
  #define TWI_MT_DATA_ACK       0x28  // Data byte has been transmitted; ACK has been received
  #define TWI_MT_DATA_NACK      0x30  // Data byte has been transmitted; NOT ACK has been received  
  // Master Receiver Mode
  #define TWI_MR_SLAR_ACK       0x40  // SLA+R has been transmitted; ACK has been received
  #define TWI_MR_SLAR_NACK      0x48  // SLA+R has been transmitted; NOT ACK has been received
  #define TWI_MR_DATA_ACK       0x50  // Data byte has been received; ACK has been received
  #define TWI_MR_DATA_NACK      0x58  // Data byte has been received; NOT ACK has been received
  
  // ++++++++++++++++++++++++++++++++++++++++++
  //
  //         S L A V E   M O D E
  //
  // ++++++++++++++++++++++++++++++++++++++++++
  // Slave Receiver Mode
  #define TWI_SR_SLAW_ACK       0x60  // Own Slave address has been received; ACK returned
  #define TWI_SR_ALMOA_ACK      0x68  // Arbitration Lost in SLA+R/W as Master; Own Slave address has been received; ACK returned
  #define TWI_SR_GCALL_ACK      0x70  // General call address has been received; ACK returned
  #define TWI_SR_ALMGA_ACK      0x78  // Arbitration lost in SLA+R/W as Master; General call address has been received; ACK returned  
  #define TWI_SR_OA_DATA_ACK    0x80  // Previously addressed with own SLA+W; data has been received; ACK returned
  #define TWI_SR_OA_DATA_NACK   0x88  // Previously addressed with own SLA+W; data has been received; NOT ACK returned
  #define TWI_SR_GC_DATA_ACK    0x90  // Previously addressed with general call; data has been received; ACK returned
  #define TWI_SR_GC_DATA_NACK   0x98  // Previously addressed with general call; data has been received; NOT ACK returned
  #define TWI_SR_STOP_RSTART    0xA0  // A STOP condition or repeated START condition has been received while still addressed as Slave
  // Slave Transmitter Mode
  #define TWI_ST_OA_ACK         0xA8  // Own SLA+R has been received; ACK has been returned
  #define TWI_ST_ALMOA_ACK      0xB0  // Arbitration lost in SLA+R/W as Master; own SLA+R has been received; ACK has been received
  #define TWI_ST_DATA_ACK       0xB8  // Data byte in TWDR has been transmitted; ACK has been received
  #define TWI_ST_DATA_NACK      0xC0  // Data byte in TWDR has been transmitted; NOT ACK has been received
  #define TWI_ST_DATA_LOST_ACK  0xC8  // Last data byte in TWDR has been transmitted (TWEA = '0'); ACK has been received
  
  /**
   * @desc    TWI init - initialise communication
   *
   * @param   void
   *
   * @return  void
   */
  void TWI_Init(void);

  /**
   * @desc    TWI MT Start
   *
   * @param   void
   *
   * @return  char
   */
  char TWI_MT_Start(void);

  /**
   * @desc    TWI Send SLAW
   *
   * @param   void
   *
   * @return  unsigned char
   */
  char TWI_MT_Send_SLAW(char);

  /**
   * @desc    TWI Send data
   *
   * @param   char
   *
   * @return  char
   */
  char TWI_MT_Send_Data(char);

  /**
   * @desc    TWI Send SLAR
   *
   * @param   void
   *
   * @return  unsigned char
   */
  char TWI_MR_Send_SLAR(char);

  /**
   * @desc    TWI stop
   *
   * @param   void
   *
   * @return  void
   */
  void TWI_Stop(void);
  
#endif
