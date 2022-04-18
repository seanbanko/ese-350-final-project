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

// MPU6050 register addresses
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

// Calibration
#define LSB_PER_G 16384.0

char str[50];
int data[6];

float acc_x;
float acc_y;
float acc_z;
float pitch;
float pitch_offset = 0;

void read_accel();
void clear_sleep_bit();

void calibrate() {
  acc_x = 0;
  acc_y = 0;
  acc_z = 0;
  for (int i = 0; i < 100; i++) {
    read_accel();
    acc_x = ((data[0] << 8) | data[1]) / LSB_PER_G;
    acc_y = ((data[2] << 8) | data[3]) / LSB_PER_G;
    acc_z = ((data[4] << 8) | data[5]) / LSB_PER_G;
    pitch += atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI);
  }
  pitch_offset = pitch / 100.0;
}

int main(void) {
  serialInit(BAUD_PRESCALER);
  twi_init();
  clear_sleep_bit();
  calibrate();
  while (1) {
    read_accel();
    acc_x = ((data[0] << 8) | data[1]) / LSB_PER_G;
    acc_y = ((data[2] << 8) | data[3]) / LSB_PER_G;
    acc_z = ((data[4] << 8) | data[5]) / LSB_PER_G;
    pitch = atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI) - pitch_offset;
    sprintf(str, "pitch: %f\n", pitch);
    serialPrint(str);
  }
}

void clear_sleep_bit() {
  twi_start();
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Send register address (RA) and wait for ACK
  TWDR = PWR_MGMT_1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Send DATA and wait for ACK
  TWDR = 0x00;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  twi_stop();
}

void read_accel() {
  twi_start();
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Send register address (RA) and wait for ACK
  TWDR = ACCEL_XOUT_H;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Send repeated start (RS) and wait for ACK
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Send device address and read bit (SLA+R) and wait for ACK + DATA
  TWDR = (MPU6050_ADDR << 1) | 1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  int i;
  for (i = 0; i < 5; i++) {
    // Send ACK
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)))
      ;
    // Store DATA
    data[i] = TWDR;
  }
  // Send NACK
  TWCR = (1 << TWINT) | (1 << TWEN);
  while (!(TWCR & (1 << TWINT)))
    ;
  // Store DATA
  data[i] = TWDR;
  twi_stop();
}
