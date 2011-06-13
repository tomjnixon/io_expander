#ifndef I2C_EXPANDER_H
#define I2C_EXPANDER_H

#include "commands.h"

#define REG_TRISC      0
#define REG_TRISB      1
#define REG_PORTC      2
#define REG_PORTB      3
#define REG_INTC       4
#define REG_INTB       5

#define PORTB   0
#define PORTC   1

#define RC0     0
#define RC1     1
#define RC2     2
#define RC5     5
#define RC6     6
#define RC7     7

#define RB0     10
#define RB1     11
#define RB2     12
#define RB3     13
#define RB4     14
#define RB5     15
#define RB6     16

#define AN0     0
#define AN1     1
#define AN2     2
#define AN3     3
#define AN4     4

typedef struct {
	uint8_t address;
	uint8_t register_contents[6];
} io_expander;

void write_command(io_expander *io, unsigned char command, unsigned char arg);
void write_reg(io_expander *io, uint8_t reg, uint8_t arg);
uint8_t read_reg(io_expander *io, uint8_t reg);
unsigned char read_byte(io_expander *io, unsigned char command);
unsigned int read_word(io_expander *io, unsigned char command);
uint8_t port_num(uint8_t pin);
uint8_t bit_num(uint8_t pin);


void init_io_expander(io_expander *io, uint8_t address);
void digitalWrite(io_expander *io, uint8_t pin, uint8_t value);
void pinMode(io_expander *io, uint8_t pin, uint8_t value);
int digitalRead(io_expander *io, uint8_t pin);
int analogRead(io_expander *io, uint8_t pin);
int attachInterrupt(io_expander *io, uint8_t pin);
int detachInterrupt(io_expander *io, uint8_t pin);

uint8_t read_port(io_expander *io, uint8_t port);
void write_port(io_expander *io, uint8_t port, uint8_t value);

#endif
