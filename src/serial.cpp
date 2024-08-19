#include "serial.h"
#include <string.h>

// Definicje zmiennych globalnych
volatile unsigned char rx_buffer[RX_BUFFER_SIZE];
volatile unsigned char rx_line[RX_LINE_SIZE];
volatile unsigned char rx_buffer_pos = 0;
volatile unsigned char rx_line_pos = 0;
volatile short waiting = 1;

void serial_init() {
    // Inicjalizacja USART
    UBRR0 = UBRR_VALUE;
    UCSR0B |= (1<<TXEN0);   // Włącz TX
    UCSR0B |= (1<<RXEN0);   // Włącz RX
    UCSR0B |= (1<<RXCIE0);  // Włącz przerwanie odbioru danych
    UCSR0C |= (1<<UCSZ01) | (1<<UCSZ01); // 8-bit dane
}

void serial_char(unsigned char data) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

void serial_int(uint8_t data) {
    while (!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}
void serial_break() {
    serial_char(10); // LF
    serial_char(13); // CR
}

void serial_string(char* s) {
    while (*s != '\0')
        serial_char(*s++);
}

ISR(USART_RX_vect) {
    if(waiting == 0)
        memset(rx_line, 0, RX_LINE_SIZE);

    waiting = 1;

    unsigned char input = UDR0;
    if(input != '\n')
        rx_line[rx_line_pos++] = input;

    if(rx_line_pos >= RX_LINE_SIZE || (input == '\n' && rx_line_pos > 0)) {
        logic_handler();
        rx_line_pos = 0;
        waiting = 0;
    }
}

void save(char* data, int len) {
    eeprom_write_block((const void *)data, (void *)0, len);
    serial_string("Data saved to EEPROM.");
    serial_string("\n----- DATA -----\n");
    serial_string(data);
    serial_string("\n----------------\n");
}

void load(char* dest, int len) {
    eeprom_read_block((void *)dest , (const void *)0 , len);
    serial_string("Data loaded from EEPROM.");
    serial_string("\n----- DATA -----\n");
    serial_string(dest);
    serial_string("\n----------------\n");
}

void logic_handler() {
    if(compare_string((char*)rx_line, "/save")) {
        save((char*)rx_buffer, RX_BUFFER_SIZE);
    }
    else if(compare_string((char*)rx_line, "/load")) {
        char data[RX_BUFFER_SIZE];
        load(data, RX_BUFFER_SIZE);
    }
    else if(compare_string((char*)rx_line, "/all")) {
        serial_string("all results: \n");
        serial_string((char*)rx_buffer);
        serial_break();
    }
    else if(rx_line[0] != '\0') {
        serial_string("input: ");
        serial_string((char*)rx_line);
        serial_break();

        short int i = 0;
        while(rx_buffer_pos + 1 < RX_BUFFER_SIZE && i < rx_line_pos)
            rx_buffer[rx_buffer_pos++] = rx_line[i++];
        rx_buffer[rx_buffer_pos++] = '\n';

        if(rx_buffer_pos >= RX_BUFFER_SIZE) {
            rx_buffer_pos = 0;
            memset((char*)rx_buffer, 0, RX_BUFFER_SIZE);
        }
    }
}

static inline int compare_string(char *first, char *second) {
    while (*first == *second && *first != '\0') {
        first++;
        second++;
    }
    return *first == '\0' && *second == '\0';
}
