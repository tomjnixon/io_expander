#include "Wire.h"

#include "i2c.h"


/////////////////////////////////////////
// The low-level command-based interface.
/////////////////////////////////////////

void write_command(io_expander *io, unsigned char command, unsigned char arg)
{
	Wire.beginTransmission(io->address);
	Wire.send(command);
	Wire.send(arg);
	Wire.endTransmission();
}

unsigned char read_byte(io_expander *io, unsigned char command)
{
	Wire.beginTransmission(io->address);
	Wire.send(command);
	Wire.endTransmission();
	
	Wire.requestFrom(io->address, 1);
	return Wire.receive();
}

unsigned int read_word(io_expander *io, unsigned char command)
{
	Wire.beginTransmission(io->address);
	Wire.send(command);
	Wire.endTransmission();
	
	Wire.requestFrom(io->address, 2);
	unsigned int a = Wire.receive();
	unsigned int b = Wire.receive();
	return (a << 8) + b;
}

/////////////////////////////////////////
// The high-level arduino-like interface.
/////////////////////////////////////////

uint8_t reg_port[] = {REG_PORTB, REG_PORTC};
uint8_t reg_tris[] = {REG_TRISB, REG_TRISC};
uint8_t reg_int[] = {REG_INTB, REG_INTC};
uint8_t cmd_set_port[] = {CMD_SET_PORTB, CMD_SET_PORTC};
uint8_t cmd_read_port[] = {CMD_READ_PORTB, CMD_READ_PORTC};
uint8_t cmd_set_tris[] = {CMD_SET_TRISB, CMD_SET_TRISC};
uint8_t cmd_set_int[] = {CMD_SET_INTB, CMD_SET_INTC};

// Set up an IO expander with a specific address.
// The default address for this one is two, so you should probably use that.
void init_io_expander(io_expander *io, uint8_t address)
{
	io->address = address;
	
	io->register_contents[REG_TRISC] = 0xFF;
	io->register_contents[REG_TRISB] = 0xFF;
	io->register_contents[REG_PORTC] = 0x0;
	io->register_contents[REG_PORTB] = 0x0;
	io->register_contents[REG_INTC] = 0x0;
	io->register_contents[REG_INTB] = 0x0;
}

uint8_t port_num(uint8_t pin)
{
	if (pin >= 10)
		return PORTB;
	else
		return PORTC;
}

uint8_t bit_num(uint8_t pin)
{
	if (pin >= 10)
		return pin - 10;
	else
		return pin;
}

void digitalWrite(io_expander *io, uint8_t pin, uint8_t value)
{
	uint8_t port = port_num(pin);
	uint8_t bit = bit_num(pin);
	
	io->register_contents[reg_port[port]] &= ~(1 << bit);
	if (value)
		io->register_contents[reg_port[port]] |= 1 << bit;
	write_command(io, cmd_set_port[port], io->register_contents[reg_port[port]]);
}

void pinMode(io_expander *io, uint8_t pin, uint8_t value)
{
	uint8_t port = port_num(pin);
	uint8_t bit = bit_num(pin);
	
	io->register_contents[reg_tris[port]] &= ~(1 << bit);
	if (!value)
		io->register_contents[reg_tris[port]] |= 1 << bit;
	write_command(io, cmd_set_tris[port], io->register_contents[reg_tris[port]]);
}

int digitalRead(io_expander *io, uint8_t pin)
{
	uint8_t port = port_num(pin);
	uint8_t bit = bit_num(pin);
	
	return read_byte(io, cmd_read_port[port]) & (1 << bit) ? 1 : 0;
}

int analogRead(io_expander *io, uint8_t pin)
{
	return read_word(io, pin);
}

int attachInterrupt(io_expander *io, uint8_t pin)
{
	uint8_t port = port_num(pin);
	uint8_t bit = bit_num(pin);
	
	io->register_contents[reg_int[port]] |= 1 << bit;
	write_command(io, cmd_set_int[port], io->register_contents[reg_int[port]]);
}

int detachInterrupt(io_expander *io, uint8_t pin)
{
	uint8_t port = port_num(pin);
	uint8_t bit = bit_num(pin);
	
	io->register_contents[reg_int[port]] &= ~(1 << bit);
	write_command(io, cmd_set_int[port], io->register_contents[reg_int[port]]);
}

///////////////////////////////////////////////////////////
// Commands for reading and writing an entire port at once.
///////////////////////////////////////////////////////////
uint8_t read_port(io_expander *io, uint8_t port)
{
	return read_byte(io, cmd_read_port[port]);
}

void write_port(io_expander *io, uint8_t port, uint8_t value)
{
	io->register_contents[reg_port[port]] = value;
	write_command(io, cmd_set_port[port], io->register_contents[reg_port[port]]);
}
