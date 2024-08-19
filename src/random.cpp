#include "random.h"
#include "adc.h"
#include <stdlib.h>
#include <util/delay.h>

void RANDOM_init(){
    uint16_t seed = 0;
    for(uint8_t i = 0; i < 16; i++){
        
        seed = (seed << 1) | (ADC_read(0) & 0x01);
        //_delay_us(seed);
    }
    srand(seed);
}
