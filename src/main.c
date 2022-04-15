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
#define PWR_MGMT_1 0x6B
#define WHO_AM_I 0x75

char str[30];


int main(void) {
  int data;

  serialInit(BAUD_PRESCALER);
  twi_init();

  sprintf(str, "after init\n");
  serialPrint(str);

  while (1) {

    sprintf(str, "top of loop\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Send start and wait for status
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while ((TWSR & 0xF8) != TWI_START)
      ;

    sprintf(str, "after start\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Send SLA+W and wait for status address sent and ack received
    TWDR = (MPU6050_ADDR << 1) | 0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while ((TWSR & 0xF8) != TWI_T_ADDR_ACK) {
    }

    sprintf(str, "after SLA+W\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Data = register address to read
    TWDR = WHO_AM_I;
    // Send it
    TWCR = (1 << TWINT) | (1 << TWEN); // send
    // Wait for status to indicate address was sent and ack received
    while ((TWSR & 0xF8) != TWI_T_DATA_ACK) {
    }

    sprintf(str, "after RA\n");
    serialPrint(str);

    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Restart
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    // Wait for status to indicate that restart has been sent
    while ((TWSR & 0xF8) != TWI_RESTART) {
    }

    sprintf(str, "after RS\n");
    serialPrint(str);

    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Data = SLA+R
    TWDR = (MPU6050_ADDR << 1) | 1;
    // Send it
    TWCR = (1 << TWINT) | (1 << TWEN);
    // Wait for status to indicate SLA+R was sent and ACK received
    while ((TWSR & 0xF8) != TWI_R_ADDR_ACK) {
    }

    sprintf(str, "after SLA+R\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    // Send NACK
    TWCR = (1 << TWINT) | (1 << TWEN);
    // Wait for status to indicate that data was received and NACK returned
    while ((TWSR & 0xF8) != TWI_R_DATA_NACK) {
    }

    sprintf(str, "after NACK sent + data recieved\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    data = TWDR;
    dataf = TWDR;

    // Stop
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    sprintf(str, "after stop\n");
    serialPrint(str);
    sprintf(str, "TWSR: %2.2x\n", TWSR);
    serialPrint(str);

    sprintf(str, "data: %i\n", data);
    serialPrint(str);

    // float roll = atan2(acc_y, acc_z) * (180 / M_PI);
    // float pitch =
    //     atan2(acc_x, sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * (180 / M_PI);
    // if (pitch > PITCH_THRESHOLD) {
    // }

    _delay_ms(500);
  }
}
