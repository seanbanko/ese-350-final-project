#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "twi.h"

// Initialize TWI (set bit rate)
void twi_init(unsigned char bit_rate) {
  TWBR = bit_rate;
}

// Send start (S) and wait
void twi_start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

// Send byte
void twi_write(unsigned char byte) {
  TWDR = byte;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

// Send repeated start (RS) and wait
void twi_repeated_start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

// Send ACK (after reading a byte)
void twi_ack() {
  // Send ACK
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
  while (!(TWCR & (1 << TWINT)))
    ;
}

// Send NACK (after reading a byte)
void twi_nack() {
  // Send ACK
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
}

// Send stop (P)
void twi_stop() { TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }
