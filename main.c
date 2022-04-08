#include <math.h>

// Registers on the MPU-6050
#define ACCEL_XOUT_H 59
#define ACCEL_XOUT_L 60
#define ACCEL_YOUT_H 61
#define ACCEL_YOUT_L 62
#define ACCEL_ZOUT_H 63
#define ACCEL_ZOUT_L 64

#define PITCH_THRESHOLD 5

void init() {
  // Configure serial communicaton via SPI or I2C
  // Configure LED
  // Configure Buzzer
}

// Read data from a given register on the MPU-6050
int serial_read(int register_num);

int main() {
  init();
  while (1) {
    float acc_x = (serial_read(ACCEL_XOUT_H) << 8) | serial_read(ACCEL_XOUT_L); 
    float acc_y = (serial_read(ACCEL_YOUT_H) << 8) | serial_read(ACCEL_YOUT_L); 
    float acc_z = (serial_read(ACCEL_ZOUT_H) << 8) | serial_read(ACCEL_ZOUT_L); 

    float roll = atan2(acc_y, acc_z) * (180 / M_PI);
    float pitch = atan2(acc_x, sqrt(pow(acc_y, 2) + pow(acc_z, 2))) * (180 / M_PI);

    if (pitch > PITCH_THRESHOLD) {
      // Light LED
      // Sound buzzer
    }
  }
}
