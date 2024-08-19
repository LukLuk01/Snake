#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdio.h>

#include "spi.h"
#include "adc.h"
#include "max7219.h"
#include "random.h"
#include "serial.h"

#define F_CPU           16000000    // clock speed
#define JOY_X 0
#define JOY_Y 1

void Point_generator(){
    
    for(uint8_t i = 0; i<64; i++){
      uint8_t point = rand() % 64;
      uint8_t x = 1 + point / 8;
      uint8_t y = point % 8;
      MAX7219_Send( x, (1<<y) );  // (x,y) na wyswietlaczu 
      _delay_ms(100);
      MAX7219_clearDisplay();
    }
}

int main(void){
    SPI_init();
    MAX7219_Init();
    ADC_init();
    RANDOM_init();
    serial_init();
    sei();
    //MAX7219_Send(1,0b10000000);
    //serial_string("Commands: \n '/save' - save all results sent via UART to EEPROM\n '/load' - load saved results from EEPROM\n");
    //serial_string(" '/all' - show buffer data \n '{DATA}' - send data");
    //serial_break();

    //Point_generator();

    char buffer[10];
    uint8_t sens = 300;
    uint8_t x_pos = 0;
    uint8_t y_pos = 0;
    uint8_t direction = 4;

  while (1){

      // Odczyt wartości z joysticka
      uint16_t xValue = ADC_read(JOY_X);  // Odczyt wartości z ADC0
      uint16_t yValue = ADC_read(JOY_Y);  // Odczyt wartości z ADC1

      if(ADC_read(JOY_X) > (512 + sens)){//lewo
        serial_string("lewo");
        direction = 1;
      }
      else if (ADC_read(JOY_X) < (512 - sens)){//prawo
        serial_string("prawo");
        direction = 0;
      }
      else if (ADC_read(JOY_Y) > (512 + sens)){
        serial_string("dol");
        direction = 2;

      }
      else if (ADC_read(JOY_Y) < (512 - sens)){
        serial_string("gora");
        direction = 3;

      }

      switch (direction)
      {
      case 0://LEFT
          x_pos=x_pos+1;
          if(x_pos>8){
            x_pos=1;
          }
        break;
      case 1://RIGHT
          x_pos=x_pos-1;
          if (x_pos<=0){
            x_pos=8;
          }
        break;
      case 2:
        y_pos = y_pos << 1; // potencialny wyciek pamieci ??? 
        if(y_pos == 0 ){
          y_pos = 0b00000001;
        }
        break;
      case 3:
        y_pos = y_pos >> 1;
        if(y_pos == 0b00000000 ){
          y_pos = 0b10000000;
        }
        break;
      default:
      direction = 4;
      x_pos = 1;
      y_pos = 128;
      break;
      }
      MAX7219_Send(x_pos,y_pos);//screen ref
      _delay_ms(400);  // Czekaj 100 ms przed kolejnym odczytem
      MAX7219_clearDisplay();

      }
  


}
