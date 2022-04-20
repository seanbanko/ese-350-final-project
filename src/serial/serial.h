#pragma once

void serial_init(int prescaler);

void serial_send(unsigned char data);

void serial_print(char *str);
