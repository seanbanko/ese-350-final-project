#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "twi.h"
#include "../serial/serial.h"

char str[30];

// Initialize TWI
void twi_init() {
  // Enable internal pull-up resistors for SCL and SDA
  // PORTC &= (1 << PORTC4) | (1 << PORTC5);
  // TODO dont make this a magic number
  // SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz	*/
  TWBR = 12;
}

// Send start (S) and wait for correct status
void twi_start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_START) {
    sprintf(str, "waiting after START\n");
    serialPrint(str);
    sprintf(str, "TWSR: %x\n", TWSR);
    serialPrint(str);
  }
}

// Send stop (P)
void twi_stop() { TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }
