#include <avr/io.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "serial/serial.h"
#include "twi/twi.h"

#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

/* MPU6050 register addresses */
#define MPU6050_ADDR 0x68
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75

char str[50];
int data[6];

float acc_x;
float acc_y;
float acc_z;
float temp;

void single_read_sequence();
void clear_sleep_bit();

int main(void) {
  serialInit(BAUD_PRESCALER);
  twi_init();

  sprintf(str, "initialization complete\n");
  serialPrint(str);

  clear_sleep_bit();

  sprintf(str, "cleared sleep bit\n");
  serialPrint(str);

  while (1) {
    single_read_sequence();
    for (int i = 0; i < 6; i++) {
      sprintf(str, "data[%d]: %08d\n", i, data[i]);
      serialPrint(str);
    }
    int accel_xout = (data[0] << 8) | data[1];
    int accel_yout = (data[2] << 8) | data[3];
    int accel_zout = (data[4] << 8) | data[5];
    int roll =
        (atan(accel_yout / sqrt(pow(accel_xout, 2) + pow(accel_zout, 2))) *
         180 / M_PI);
    int pitch =
        (atan(-1 * accel_xout / sqrt(pow(accel_yout, 2) + pow(accel_zout, 2))) *
         180 / M_PI);
  }
}

void clear_sleep_bit() {
  twi_start();
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after SLA+W\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Send register address (RA) and wait for ACK
  TWDR = PWR_MGMT_1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after RA\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Send DATA and wait for ACK
  TWDR = 0x00;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after DATA\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  twi_stop();
  _delay_ms(5);
}

void single_read_sequence() {
  twi_start();
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after SLA+W\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Send register address (RA) and wait for ACK
  TWDR = ACCEL_XOUT_H;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after RA\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Send repeated start (RS) and wait for ACK
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after RS\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Send device address and read bit (SLA+R) and wait for ACK + DATA
  TWDR = (MPU6050_ADDR << 1) | 1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after SLA+R\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  int i;
  for (i = 0; i < 5; i++) {
    // Send ACK
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT))) {
      // sprintf(str, "waiting after ACK\n");
      // serialPrint(str);
      // sprintf(str, "TWSR: %x\n", TWSR);
      // serialPrint(str);
    }
    // Store DATA
    data[i] = TWDR;
  }
  // Send NACK
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT))) {
    // sprintf(str, "waiting after NACK\n");
    // serialPrint(str);
    // sprintf(str, "TWSR: %x\n", TWSR);
    // serialPrint(str);
  }
  // Store DATA
  data[i] = TWDR;

  twi_stop();
}
