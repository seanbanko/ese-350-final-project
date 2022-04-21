#include <avr/io.h>
#include <avr/interrupt.h>
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

// Buzzer
#define PITCH_THRESHOLD 5
#define COUNT_2500HZ 199
#define DUTY_CYCLE 0.1

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

int timer2Periods = 0;

void mpu6050_read_accel();
void mpu6050_init();
void mpu6050_calibrate();

void buzzer_init();
void buzzer_enable();
void buzzer_disable();

void init() {
  cli();
  serial_init(BAUD_PRESCALER);
  twi_init(TWI_BIT_RATE);
  mpu6050_init();
  mpu6050_calibrate();
  buzzer_init();
  sei();
}

int main(void) {
  // float acc_z_old;
  while (1) {
    // acc_z_old = acc_z;
    mpu6050_read_accel();
    acc_x = ((data[0] << 8) | data[1]) / LSB_PER_G;
    acc_y = ((data[2] << 8) | data[3]) / LSB_PER_G;
    acc_z = ((data[4] << 8) | data[5]) / LSB_PER_G;
    pitch = atan2(acc_x, sqrt(acc_y * acc_y + acc_z * acc_z)) * (180 / M_PI) -
            pitch_offset;
    pitch_acc += pitch;
    min_acc_z = fminf(acc_z, min_acc_z);
    max_acc_z = fmaxf(acc_z, max_acc_z);

    if (fabs(pitch) > PITCH_THRESHOLD) {
      buzzer_enable();
    }

    // Sends data to NodeMCU
    int16_t i = pitch;
    serial_send_i16(i);

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

void buzzer_init() {
  // Sets PD3 as output
  DDRD |= (1 << DDD3);
  // Prescales Timer 2 by 32
  TCCR2B |= (1 << CS20);
  TCCR2B |= (1 << CS21);
  // Sets Timer 2 mode to Fast PWM
  TCCR2A |= (1 << WGM20);
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << WGM22);
  // Sets Timer 2 Output Compare Register 2A for 2.5 kHz
  OCR2A = COUNT_2500HZ;
  // Sets Timer 2 Output Compare Register 2B for 5% duty cycle
  OCR2B = DUTY_CYCLE * OCR2A;
  TIMSK2 |= (1 << OCIE2A);
}

void buzzer_enable() {
  timer2Periods = 0;
  // Sets Timer 2 to clear 0C2B on Compare Match B, reset at bottom
  TCCR2A |= (1 << COM2B1);
}

void buzzer_disable() {
  // Sets Timer 2 disable action of OC2B
  TCCR2A &= ~(1 << COM2B1);
  PORTD &= ~(1 << PORTD3);
}

ISR(TIMER2_COMPA_vect) {
  if (++timer2Periods > 200) {
    buzzer_disable();
    timer2Periods = 0;
  }
}
