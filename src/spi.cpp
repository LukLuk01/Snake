#include "spi.h"

void SPI_init(){
    DDRB |= (1 << SCK_PIN) | (1 << MOSI_PIN) | (1 << SS_PIN);  // SCK, MOSI, CS/LOAD/SS

    // SPI Enable, Master mode
    SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR1);
}


void SPI_Transmit(uint8_t data) {
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));  // Wait for transmission to complete
}
