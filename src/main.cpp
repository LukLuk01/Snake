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
#define sens 400

#define SNAKE_MAX_LENGTH 16

typedef struct {
    uint8_t x;  // Kolumna (1-8)
    uint8_t y;  // Bit reprezentujący pozycję w wierszu (1<<y)
} Point;

Point snake[SNAKE_MAX_LENGTH];
uint8_t snake_length = 3;
Point food;

uint8_t direction = 0; // 0 - prawo, 1 - lewo, 2 - dół, 3 - góra

void generate_food() {
    food.x = rand() % 8 + 1;  // Zakres kolumn (1-8)
    food.y = 1 << (rand() % 8);  // Zakres wierszy (bitowo)
}

void init_snake_game() {
    snake_length = 3;
    snake[0].x = 4; snake[0].y = (1 << 4);  // Środek planszy
    snake[1].x = 3; snake[1].y = (1 << 4);
    snake[2].x = 2; snake[2].y = (1 << 4);
    generate_food();
}

void move_snake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    switch (direction) {
        case 0: // prawo
            snake[0].x++;
            if (snake[0].x > 8) snake[0].x = 1;
            break;
        case 1: // lewo
            snake[0].x--;
            if (snake[0].x < 1) snake[0].x = 8;
            break;
        case 2: // dół (odwrócony)
            if (snake[0].y == (1 << 7)) {
                snake[0].y = 1;  // Przejście do górnego wiersza
            } else {
                snake[0].y <<= 1;  // Przesunięcie bitowe w dół (odwrócone)
            }
            break;
        case 3: // góra (odwrócony)
            if (snake[0].y == 1) {
                snake[0].y = 1 << 7;  // Przejście do dolnego wiersza
            } else {
                snake[0].y >>= 1;  // Przesunięcie bitowe w górę (odwrócone)
            }
            break;
    }
}

int check_collision() {
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1; // Kolizja z ciałem węża
        }
    }
    return 0;
}

void update_snake() {
    if (snake[0].x == food.x && snake[0].y == food.y) {
        snake_length++;
        if (snake_length > SNAKE_MAX_LENGTH) snake_length = SNAKE_MAX_LENGTH;
        generate_food();
    } else {
        move_snake();
    }
}

void draw_snake() {
    uint8_t display[8] = {0};  // Bufor reprezentujący każdą kolumnę wyświetlacza

    // Wypełnienie bufora pozycjami węża
    for (int i = 0; i < snake_length; i++) {
        display[snake[i].x - 1] |= snake[i].y;
    }

    // Dodanie owocu do odpowiedniej kolumny
    display[food.x - 1] |= food.y;

    // Wysłanie zawartości bufora na wyświetlacz
    for (int i = 0; i < 8; i++) {
        MAX7219_Send(i + 1, display[i]);
    }
}


int main(void) {
    SPI_init();
    MAX7219_Init();
    ADC_init();
    RANDOM_init();
    serial_init();
    sei();

    init_snake_game();

    while (1) {
        uint16_t xValue = ADC_read(JOY_X);
        uint16_t yValue = ADC_read(JOY_Y);

        if (xValue > (512 + sens)) {
            direction = 1;  // lewo
        } else if (xValue < (512 - sens)) {
            direction = 0;  // prawo
        } else if (yValue > (512 + sens)) {
            direction = 2;  // dół (odwrócony)
        } else if (yValue < (512 - sens)) {
            direction = 3;  // góra (odwrócony)
        }

        update_snake();

        if (check_collision()) {
            serial_string("Game Over\n");
            _delay_ms(2000);
            init_snake_game();
        }

        draw_snake();
        _delay_ms(400);
    }
}
