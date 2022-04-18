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

void read_accel();
void clear_sleep_bit();

int main(void) {
  serialInit(BAUD_PRESCALER);
  twi_init();
  clear_sleep_bit();

  while (1) {
    read_accel();
    // for (int i = 0; i < 6; i++) {
    //   sprintf(str, "data[%d]: %08d\n", i, data[i]);
    //   serialPrint(str);
    // }
    int acc_x_raw = (data[0] << 8) | data[1];
    int acc_y_raw = (data[2] << 8) | data[3];
    int acc_z_raw = (data[4] << 8) | data[5];
    float acc_x = acc_x_raw / LSB_PER_G;
    float acc_y = acc_y_raw / LSB_PER_G;
    float acc_z = acc_z_raw / LSB_PER_G;
    // sprintf(str, "%f / ", acc_x);
    // serialPrint(str);
    // sprintf(str, "%f / ", acc_y);
    // serialPrint(str);
    // sprintf(str, "%f\n", acc_z);
    // serialPrint(str);
    float roll = atan2(acc_y, acc_z) * (180 / M_PI);
    float pitch =
        atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI);
    if (pitch > 10) {
      sprintf(str, "PITCH: %f\n", pitch);
      serialPrint(str);
    }
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
