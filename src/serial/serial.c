#include <avr/io.h>
#include <stdint.h>

#include "serial.h"

void serial_init(int BAUD_PRESCALER) {
  // Set baud rate
  UBRR0H = (unsigned char)(BAUD_PRESCALER >> 8);
  UBRR0L = (unsigned char)BAUD_PRESCALER;
  // Enable receiver and transmitter
  UCSR0B = (1 << RXEN0) | (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits
  UCSR0C |= (1 << USBS0);                 // 2 stop bits
}

void serial_send_byte(unsigned char data) {
  // Wait for empty transmit buffer
  while (!(UCSR0A & (1 << UDRE0)))
    ;
  // Put data into buffer and send data
  UDR0 = data;
}

void serial_send_i16(int16_t i) {
  serial_send_byte(i >> 8);
  serial_send_byte(i & 0x00FF);
}

void serial_print(char *str) {
  while (*str) {
    serial_send_byte(*str);
    str++;
  }
}
