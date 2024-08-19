#include "max7219.h"

void MAX7219_Send(uint8_t address, uint8_t data) {
    
    MAX7219_LOAD0;  // Select MAX7219 (CS low)
    SPI_Transmit(address);
    SPI_Transmit(data);
    MAX7219_LOAD1;  // Deselect MAX7219 (CS high)
}

void MAX7219_clearDisplay() {
    char i = digitsInUse;
    do {
        MAX7219_Send(i, 0x00);  // Set each display in use to blank
    } while (--i);
}

void MAX7219_Init() {
    MAX7219_Send(MAX7219_MODE_DECODE, 0x00);  // Decode mode: none
    MAX7219_Send(MAX7219_MODE_INTENSITY, 0x01);  // Intensity: 1 (0x0F for max brightness)
    MAX7219_Send(MAX7219_MODE_SCAN_LIMIT, 0x07);  // Scan limit: 8 LEDs
    MAX7219_Send(MAX7219_MODE_POWER, 0x01);  // Shutdown register: normal operation
    MAX7219_Send(MAX7219_MODE_TEST, 0x00);  // Display test: off

    MAX7219_clearDisplay();
}
