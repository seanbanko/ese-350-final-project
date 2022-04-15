#include "serial.h"
#include <avr/io.h>

void serialInit(int BAUD_PRESCALER) {
  // Set baud rate
  UBRR0H = (unsigned char)(BAUD_PRESCALER >> 8);
  UBRR0L = (unsigned char)BAUD_PRESCALER;
  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  // Set frame format: 2 stop bits, 8 data bits
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
  UCSR0C |= (1 << USBS0);                 // 2 stop bits
}

void serialSend(unsigned char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  // Put data into buffer and send data
  UDR0 = data;
}

void serialPrint(char *str) {
  while (*str) {
    serialSend(*str);
    str++;
  }
}
