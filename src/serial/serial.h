#pragma once

#include <stdint.h>

void serial_init(int prescaler);
void serial_send_byte(unsigned char data);
void serial_send_i16(int16_t i);
void serial_print(char *str);
