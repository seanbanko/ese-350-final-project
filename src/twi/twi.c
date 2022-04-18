#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "twi.h"
#include "../serial/serial.h"

char str2[30];

// Initialize TWI
void twi_init() {
  // Enable internal pull-up resistors for SCL and SDA
  // PORTC |= (1 << PORTC4) | (1 << PORTC5);
  // TODO dont make this a magic number
  // SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz	*/
  TWBR = 72;
}

// Send start (S) and wait for correct status
void twi_start() {
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str2, "waiting after START\n");
    // serialPrint(str2);
    // sprintf(str2, "TWSR: %x\n", TWSR);
    // serialPrint(str2);
  }
}

// Send stop (P)
void twi_stop() { TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); }
