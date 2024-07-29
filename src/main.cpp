#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL

/*
clk --- 10 -- PB2 -- SS   -- PCINT2 -- OC1B
cs  --- 11 -- PB3 -- MOSI -- PCINT3 -- OC2A
din --- 12 -- PB4 -- MISO -- PCINT4 --

             ____
        VCC |     1   28 | GND
 (D 0) PB0     |  2   27 | PC0 (A 0 / D14)
 (D 1) PB1     |  3   26 | PC1 (A 1 / D15)
*SS (D10) PB2  |  4   25 | PC2 (A 2 / D16)
MOSI (D11) PB3 |  5   24 | PC3 (A 3 / D17)
MISO (D12) PB4 |  6   23 | PC4 (A 4 / D18)
 SCK (D13) PB5 |  7   22 | PC5 (A 5 / D19)
      XTAL1    |  8   21 | AREF
      XTAL2    |  9   20 | AVCC
 (D 5) PD3     | 10   19 | PB6 (D 6 / XTAL1)
 (D 6) PD4     | 11   18 | PB7 (D 7 / XTAL2)
 (D 7) PD5     | 12   17 | PD6 (D 8)
 (D 8) PD6     | 13   16 | PD7 (D 9)
 (D 9) PD7     | 14   15 | PC6 (RESET)

             ----*/


#define SCK_PIN     PORTB5  // sprzetowe piny do komunikacji 
#define MOSI_PIN    PORTB3
#define SS_PIN      PORTB2

#define MAX7219_LOAD1             PORTB |= (1<<SS_PIN)  // wysoki master koniec komunikacji
#define MAX7219_LOAD0             PORTB &= ~(1<<SS_PIN) // niski poczatek komunikacji 

#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_POWER        0x0C
#define MAX7219_MODE_TEST         0x0F
#define MAX7219_MODE_NOOP         0x00

#define MAX7219_CHAR_BLANK        0xF 

#define MAX7219_DIGIT0            0x01
#define MAX7219_DIGIT1            0x02
#define MAX7219_DIGIT2            0x03
#define MAX7219_DIGIT3            0x04
#define MAX7219_DIGIT4            0x05
#define MAX7219_DIGIT5            0x06
#define MAX7219_DIGIT6            0x07
#define MAX7219_DIGIT7            0x08

#define JOY_X           1  // Pin ADC0
#define JOY_Y           0  // Pin ADC1

char digitsInUse = 8;

void SPI_Transmit(uint8_t data) {
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));  // Czekaj na zakończenie transmisji
}

void MAX7219_Send(uint8_t address, uint8_t data) {
    MAX7219_LOAD0;  // Wybór układu MAX7219 (CS w stanie niskim)
    SPI_Transmit(address);
    SPI_Transmit(data);
    MAX7219_LOAD1;  // Koniec transmisji (CS w stanie wysokim)
}

void MAX7219_clearDisplay() 
{
    char i = digitsInUse;
    // Loop until 0, but don't run for zero
    do {
        // Set each display in use to blank
        MAX7219_Send(i, 0x00);
    } while (--i);
}

void MAX7219_Init() {
    MAX7219_Send(0x09, 0x00);  // Decode mode: none
    MAX7219_Send(0x0A, 0x01);  // Intensity: max (0x0F for max brightness)
    MAX7219_Send(0x0B, 0x07);  // Scan limit: 8 LEDs
    MAX7219_Send(0x0C, 0x01);  // Shutdown register: normal operation
    MAX7219_Send(0x0F, 0x00);  // Display test: off
}

void SPI_init(){

      // SCK MOSI CS/LOAD/SS
    DDRB |= (1 << SCK_PIN) | (1 << MOSI_PIN) | (1 << SS_PIN);

    // SPI Enable, Master mode
    SPCR |= (1 << SPE) | (1 << MSTR)| (1<<SPR1);
}

void ADC_init(){
  ADMUX = (1 << REFS0);  // Referencja AVCC z kondensatorem na AREF
  ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 8 - // 125KHz sample rate 1MHz

  ADCSRA |= (1 << ADEN); // Enable ADC
}

uint16_t ADC_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);  // Wybór kanału ADC
    ADCSRA |= (1 << ADSC);  // Rozpoczęcie konwersji
    while (ADCSRA & (1 << ADSC));  // Czekaj na zakończenie konwersji

    _delay_us(10);

    return ADC;
}


int main(void){
    SPI_init();
    MAX7219_Init();
    MAX7219_clearDisplay();
    ADC_init();


  while (1){
      // Odczyt wartości z joysticka
      uint16_t xValue = ADC_read(JOY_X);  // Odczyt wartości z ADC0
      uint16_t yValue = ADC_read(JOY_Y);  // Odczyt wartości z ADC1

      // Mapowanie wartości joysticka do zakresu 0-7
      uint8_t xMapped = (uint8_t)((xValue * 8) / 1023);  // Przybliżone mapowanie
      uint8_t yMapped = (uint8_t)((yValue * 8) / 1023);  // Przybliżone mapowanie
      
      // Wypełnianie matrycy LED wzorem opartym na wartości joysticka
      uint8_t pattern[8] = {0};
      pattern[yMapped] = (1 << xMapped);  // Ustawienie odpowiedniego bitu w wierszu

      for (uint8_t i = 0; i < 8; i++) {
          MAX7219_Send(i + 1, pattern[i]);  // Wyślij wzór do MAX7219
      }

      _delay_ms(100);  // Czekaj 100 ms przed kolejnym odczytem
      }
  


}
