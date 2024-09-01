#include "adc.h"


#include <util/delay.h>

void ADC_init(){
    ADMUX = (1 << REFS0);  // Reference AVCC with external capacitor at AREF pin
    ADCSRA |= (0 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC prescaler 8 (125KHz sample rate for 1MHz)
    ADCSRA |= (1 << ADEN); // Enable ADC
}

uint16_t ADC_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);  // Select ADC channel
    ADCSRA |= (1 << ADSC);  // Start conversion
    while (ADCSRA & (1 << ADSC));  // Wait for conversion to complete
    _delay_us(10);

    return ADC;
}
