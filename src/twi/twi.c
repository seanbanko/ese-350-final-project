#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "twi.h"

void twi_init() {
  // Enable internal pull-up resistors for SCL and SDA
  PORTC &= (1 << PORTC4) | (1 << PORTC5);
  // TODO dont make this a magic number
  // SCL_freq = 16MHz/(16 + 2*12*1) = 400KHz	*/
  TWBR = 12;
}
