#include "avr.h"

void
avr_init(void)
{
	WDTCR = 15;
}

void
avr_wait(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

void
avr_wait_2(unsigned short msec)
{
    TCCR0 = 2;
    while (msec--) {
        TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 8) * 0.000004);
        SET_BIT(TIFR, TOV0);
        WDR();
        while (!GET_BIT(TIFR, TOV0));
    }
    TCCR0 = 0;
}