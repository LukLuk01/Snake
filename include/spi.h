#ifndef SPI_H
#define SPI_H

#include <avr/io.h>

#define SCK_PIN  PORTB5  // hardware pins for communication 
#define MOSI_PIN PORTB3
#define SS_PIN   PORTB2

#define MAX7219_LOAD1 PORTB |= (1<<SS_PIN)  // master end communication
#define MAX7219_LOAD0 PORTB &= ~(1<<SS_PIN) // master start communication

void SPI_init(void);
void SPI_Transmit(uint8_t data);

#endif // SPI_H
