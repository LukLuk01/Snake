#ifndef SERIAL_H
#define SERIAL_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>

// Definicje
#define USART_BAUDRATE  9600
#define UBRR_VALUE      (((F_CPU/(USART_BAUDRATE*16UL)))-1)
#define RX_BUFFER_SIZE  512
#define RX_LINE_SIZE    128

// Zmienne globalne
extern volatile unsigned char rx_buffer[RX_BUFFER_SIZE];
extern volatile unsigned char rx_line[RX_LINE_SIZE];
extern volatile unsigned char rx_buffer_pos;
extern volatile unsigned char rx_line_pos;
extern volatile short waiting;

// Deklaracje funkcji
void serial_init();
void serial_char(unsigned char data);
void serial_int(uint8_t data);

void serial_break();
void serial_string(char* s);
void save(char* data, int len);
void load(char* dest, int len);
void logic_handler();
static inline int compare_string(char *first, char *second);

#endif
