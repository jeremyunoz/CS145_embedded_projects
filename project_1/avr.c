/**
 * avr.c
 * Copyright (C) 2001-2020, Tony Givargis
 */

#include "avr.h"

void avr_wait(unsigned short msec) {
  TCCR0 = 3;
  while (msec--) {
    TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
    SET_BIT(TIFR, TOV0);
    while (!GET_BIT(TIFR, TOV0))
      ;
  }
  TCCR0 = 0;
}

// instruction purpose
void wait(void) {
  volatile int i;
  for (i = 0; i < 10000; i++) {
  }
}

int main(void) {
  // Configuration (once)
  SET_BIT(DDRB, 1);  // set port B pin 1 as the output
  CLR_BIT(DDRA, 0);  // set port A pin 0 as the input
  CLR_BIT(PORTB, 1); // set port B pint 1 to low

  // The running process
  while (1) {
    if (!GET_BIT(PINA, 0)) { // When button is pressed
      SET_BIT(PORTB, 1);
      avr_wait(500);
      CLR_BIT(PORTB, 1);
      avr_wait(500);
    } else {
      CLR_BIT(PORTB, 1); // When button is not pressed
    }
  }
  return 0;
}
