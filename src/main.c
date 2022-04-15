#include <avr/io.h>
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
int data;

float acc_x;
float acc_y;
float acc_z;
float temp;

void single_read_sequence();
void single_write_sequence();

int main(void) {

  serialInit(BAUD_PRESCALER);
  twi_init();

  sprintf(str, "after twi init\n");
  serialPrint(str);

  single_write_sequence();
  sprintf(str, "after write sequence\n");
  serialPrint(str);

  while (1) {
    single_read_sequence();

    // float roll = atan2(acc_y, acc_z) * (180 / M_PI);
    // float pitch =
    //     atan2(acc_x, sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * (180 / M_PI);
    // if (pitch > PITCH_THRESHOLD) {
    // }

    _delay_ms(500);
  }
}

void single_write_sequence() {
  twi_start();
  sprintf(str, "test\n");
  serialPrint(str);
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_T_ADDR_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send register address (RA) and wait for ACK
  TWDR = PWR_MGMT_1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_T_DATA_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send DATA and wait for ACK
  TWDR = 0x00;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_T_DATA_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  twi_stop();
  sprintf(str, "test\n");
  serialPrint(str);
}

void single_read_sequence() {
  twi_start();
  sprintf(str, "test\n");
  serialPrint(str);
  // Send device address and write bit (SLA+W) and wait for ACK
  TWDR = (MPU6050_ADDR << 1) | 0;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_T_ADDR_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send register address (RA) and wait for ACK
  TWDR = ACCEL_XOUT_H;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_T_DATA_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send repeated start (RS) and wait for ACK
  TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_RSTART)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send device address and read bit (SLA+R) and wait for ACK + DATA
  TWDR = (MPU6050_ADDR << 1) | 1;
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_R_ADDR_ACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Send NACK
  TWCR = (1 << TWINT) | (1 << TWEN);
  while ((TWSR & 0xF8) != TWI_R_DATA_NACK)
    ;
  sprintf(str, "test\n");
  serialPrint(str);
  // Store DATA
  data = TWDR;

  twi_stop();

  sprintf(str, "raw data: %08d\n", data);
  serialPrint(str);

  sprintf(str, "data << 8: %08d\n", (data << 8));
  serialPrint(str);

  acc_x = (data << 8) / 16384.0;

  sprintf(str, "acc_x: %f\n", acc_x);
  serialPrint(str);
}
