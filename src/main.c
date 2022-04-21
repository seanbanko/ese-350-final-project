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
#define LSB_PER_G 16384.0

// TWI
#define TWI_BIT_RATE 72

// MISC
#define PITCH_THRESHOLD 5 

char str[50];
int data[6];

float acc_x;
float acc_y;
float acc_z;
float pitch;
float pitch_offset = 0;

float pitch_acc = 0;
float max_acc_z = -INFINITY;
float min_acc_z = INFINITY;

void mpu6050_read_accel();
void mpu6050_init();
void mpu6050_calibrate();

int main(void) {
  serial_init(BAUD_PRESCALER);
  twi_init(TWI_BIT_RATE);
  mpu6050_init();
  mpu6050_calibrate();
  // float acc_z_old;
  while (1) {
    // acc_z_old = acc_z;
    mpu6050_read_accel();
    acc_x = ((data[0] << 8) | data[1]) / LSB_PER_G;
    acc_y = ((data[2] << 8) | data[3]) / LSB_PER_G;
    acc_z = ((data[4] << 8) | data[5]) / LSB_PER_G;
    pitch = atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI) - pitch_offset;
    pitch_acc += pitch;
    min_acc_z = fminf(acc_z, min_acc_z);
    max_acc_z = fmaxf(acc_z, max_acc_z);

    if (fabs(pitch) > PITCH_THRESHOLD) {
      // TODO Buzzer
    }

    // Sends data to NodeMCU
    int16_t i = pitch;
    serial_send_i16(i);

    // Code for trying to detect reps
    // if (fabs(acc_z - acc_z_old) > 0.5) {
    //   if (acc_z > acc_z_old) {
    //     sprintf(str, "UP\n");
    //     serialPrint(str);
    //   } else {
    //     sprintf(str, "DOWN\n");
    //     serialPrint(str);
    //   }
    // }

    _delay_ms(100);
  }
}

void mpu6050_init() {
  twi_start();
  twi_write((MPU6050_ADDR << 1) | 0);
  twi_write(PWR_MGMT_1);
  twi_write(0x00);
  twi_stop();
}

void mpu6050_calibrate() {
  for (int i = 0; i < 1000; i++) {
    mpu6050_read_accel();
    acc_x = ((data[0] << 8) | data[1]) / LSB_PER_G;
    acc_y = ((data[2] << 8) | data[3]) / LSB_PER_G;
    acc_z = ((data[4] << 8) | data[5]) / LSB_PER_G;
    pitch += atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI);
  }
  pitch_offset = pitch / 1000.0;
}

void mpu6050_read_accel() {
  twi_start();
  // Send device address and write bit (SLA+W) and wait for ACK
  twi_write((MPU6050_ADDR << 1) | 0);
  // Send register address (RA) and wait for ACK
  twi_write(ACCEL_XOUT_H);
  // Send repeated start (RS) and wait for ACK
  twi_repeated_start();
  // Send device address and read bit (SLA+R) and wait for ACK + DATA
  twi_write((MPU6050_ADDR << 1) | 1);
  int i = 0;
  for (i = 0; i < 5; i++) {
    twi_ack();
    data[i] = TWDR;
  }
  twi_nack();
  data[i] = TWDR;
  twi_stop();
}
