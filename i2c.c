#define __16f876a
#include "pic16f876a.h"

#include "commands.h"

/* Setup chip configuration */
typedef unsigned int config;
config __at 0x2007 __CONFIG = _CP_OFF & 
	_WDT_OFF & 
	_BODEN_OFF & 
	_PWRTE_OFF & 
	_RC_OSC &
	_LVP_OFF;

unsigned char int_c = 0;
unsigned char int_b = 0;
unsigned char last_c = 0;
unsigned char last_b = 0;

unsigned int portb = 0;

unsigned char state = 0;

#define BUFFER_LEN 2
unsigned char buffer[BUFFER_LEN];
unsigned char buffer_size = 0;

unsigned char buffer_pop(void)
{
	unsigned char head = buffer[0];
	unsigned char i;
	if (buffer_size == 0)
		return 0;
	
	for (i = 1; i < buffer_size; i++)
		buffer[i-1] = buffer[i];
	buffer_size--;
	return head;
}

void analogue_read(unsigned char port)
{
	unsigned char i;
	
	ADCON0 = 0b11000001 | (port & 0b111) << 3;
	ADCON1 = 0b11000000;
	for (i = 0; i < 100; i++)
		__asm
			nop
		__endasm;
	GO = 1;
	while (NOT_DONE)
		;
	buffer[0] = ADRESH;
	buffer[1] = ADRESL;
	buffer_size = 2;
}
	
void handle_command(unsigned char command)
{
	switch(state) {
		case 0:
			switch(command) {
				case CMD_READ_AN0:
				case CMD_READ_AN1:
				case CMD_READ_AN2:
				case CMD_READ_AN3:
				case CMD_READ_AN4:
					analogue_read(command);
					break;
				case CMD_READ_PORTC:
					buffer[0] = PORTC;
					buffer_size = 1;
					portb = portb & 0x7F;
					PORTB = portb;
					break;
				case CMD_READ_PORTB:
					buffer[0] = PORTB;
					buffer_size = 1;
					portb = portb & 0x7F;
					PORTB = portb;
					break;
				case CMD_SET_TRISC:
				case CMD_SET_TRISB:
				case CMD_SET_PORTC:
				case CMD_SET_PORTB:
				case CMD_SET_INTC:
				case CMD_SET_INTB:
					state = command;
			}
			break;
		case CMD_SET_TRISC:
			// RC3 & RC4 are used for I2C, and must be outputs.
			TRISC = command | 0b00011000;
			break;
		case CMD_SET_TRISB:
			// The interrupt out pin in RB7.
			TRISB = command & 0x7F;
			break;
		case CMD_SET_PORTC:
			PORTC = command;
			break;
		case CMD_SET_PORTB:
			portb = command;
			PORTB = portb;
			break;
		case CMD_SET_INTC:
			int_c = command;
			break;
		case CMD_SET_INTB:
			int_b = command;
			break;
	}
}

volatile unsigned char address = 0;

static void isr(void) __interrupt 0 { 
	unsigned char buf;
	if (SSPIF) {

		buf = SSPBUF;
		address = SSPBUF;

		if (I2C_READ) {
			if (buffer_size)
				SSPBUF = buffer_pop();
			if (SSPOV) {
				TRISB = 0x7E;
				PORTB = 1;
			}
		} else {
			// process command
			if (I2C_DATA) {
				handle_command(buf);
				if (SSPOV) {
					TRISB = 0x7D;
					PORTB = 2;
				}
			}
			else {
				__asm
					MOVF	_SSPBUF, W
				__endasm;
				address = buf;
				state = 0;
				if (SSPOV) {
					TRISB = 0x7B;
					PORTB = 4;
				}
			}
		BF = 0;
		}



		SSPIF = 0;
		CKP = 1;
	}
}

void main(void) {

	TRISA = 0xFF; 
	TRISB = 0x7F;
	NOT_RBPU = 0;
	portb = 0;
	PORTB = portb;
	TRISC = 0xFF;
	
	SSPADD = 4;
	WCOL = 0;
	SSPOV = 0;
	BF = 0;
	CKP = 1;
	SSPM0 = 0;
	SSPM1 = 1;
	SSPM2 = 1;
	SSPM3 = 0;
	GCEN = 0;
	SEN = 1;
	
	
	SSPEN = 1;
	
	
	INTCON = 0;             /* clear interrupt flag bits */
	PEIE = 1;
	SSPIE = 1;
	
	GIE = 1;                /* global interrupt enable */

	while (1) {
		if (SSPOV) {
			TRISB = 0;
			PORTB = SSPBUF;
			while (1) {}
		}
		// if (((last_c ^ PORTC) & int_c) || ((last_b ^ PORTB) & int_b)) {
		// 	portb = portb | 0x80;
		// 	PORTB = portb;
		// 
		// 	last_c = PORTC;
		// 	last_b = PORTB;
		// }
	}
	
}

