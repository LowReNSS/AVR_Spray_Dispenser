/*
 * AVR_Spray_Dispenser.c
 *
 * Created: 01.01.2023 16:15:44
 * Author : Schumacher
 */ 

// ATtiny13
// Fuses L: 0x77
// Fuses H: 0xFF

#define F_CPU 128000UL

#include <avr/io.h>
#include <util/delay.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

volatile uint16_t sec_cnt = 3600;


#define LIGHT_IS_OFF() (PINB & (1 << PINB3))

ISR(WDT_vect)
{
	sec_cnt += 8;
}

int main(void)
{
	// Disable not used pin
	DDRB	&= ~((1 << PINB0) | (1 << PINB1) | (1 << PINB2));
	PORTB	|=  ((1 << PINB0) | (1 << PINB1) | (1 << PINB2));
	
	// Configure light sensor pin
	DDRB	&= ~(1 << PINB3);
	PORTB	|=  (1 << PINB3);
	
	// Configure damper pin
	DDRB	|=  (1 << PINB4);
	PORTB	&=  ~(1 << PINB4);
	
	// Disable ADC
	ADCSRA &= ~(1 << ADEN);

	// Configure WDT
	MCUSR &= ~(1 << WDRF);
	WDTCR |= (1 << WDCE) | (1 << WDE);
	WDTCR  = (1 << WDP3) | (0 << WDP2) | (0 << WDP1) | (1 << WDP0);	// Period 8 sec
	WDTCR |= (1 << WDTIE); // Enable the WDT interrupt.
	sei();

    while(1) 
    {
		if((LIGHT_IS_OFF()) && (sec_cnt > 3600))
		{
			PORTB |=   (1 << PINB4);
			_delay_ms(700);
			PORTB &=  ~(1 << PINB4);
			sec_cnt = 0;
		}
		
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_mode(); // Start sleep mode
		
    }
}