# ESE 350 Final Project: Group 29 Squat Support

## DevPost and Links

Check out our [DevPost](https://devpost.com/software/squat-support) and our [YouTube video](https://urldefense.com/v3/__https://www.youtube.com/watch?v=DHqvz-y0gxQ__;!!IBzWLUs!SMsAatfbAjFeWANAP97uEC9RmyY04_OxHeGKsir70sWGwWuoxGzvDhCCM_AKMuogv5P7Sa_hxHmNsP6iVYC6tVAO$)

## How to Compile and Run this Code:

Compile and flash src to the Atmega328p with the AVR tooltain for gcc. 

Compile and flash blynk.ino to the ESP8266

## Structure of Project: (generated with `tree`)

```
src The code running on the Atmega328p.

├── main.c : the main file containing the code running on our Atmega328p.

├── serial

│   ├── serial.c : defines functions for UART (reused from Lab 3)

│   └── serial.h : declares functions for UART (reused from Lab 3)

└── twi

    ├── twi.c : defines functions for TWI (modeled heavily on the example in Table 21.2 from the datasheet for the Atmega328p)

    └── twi.h : defines status codes for TWI (Section 21.7 in the datasheet for the Atmega328p) and declares functions

blynk.ino: The code running on the ESP8266.
```
