# ESE 350 Final Project: Group 29 Squat Support

Structure of Code:

src The code running on the Atmega328p.
├── main.c : the main file containing the code running on our Atmega328p.
├── serial
│   ├── serial.c : defines functions for UART (again, reused from Lab 3)
│   └── serial.h : declares functions for UART
└── twi
    ├── twi.c : defines status codes for TWI (Section 21.7 in the datasheet) and declares functions
    └── twi.h : defines functions for TWI (modeled heavily on the example in Table 21.2 from the datasheet)

blynk.ino: The code running on the ESP8266.
