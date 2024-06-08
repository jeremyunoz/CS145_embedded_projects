#include <stdio.h>
#include "lcd.h"
#include "avr.h"

unsigned long sum = 0;
signed long diff_sum = 0;
signed int diff_max = 0;
signed int diff_min = 0;

int count = 0;
int max = 0;
int min = 0;

volatile int need_sample = 0;	// if we need sample, call printSample, otherwise just get sample
volatile int reset = 0;
volatile int need_differential_sample = 0; 
volatile int current_mode = 0; // default: normal sampling

int is_pressed(int r, int c)
{
	// all 8 GPIOs to N / C
	DDRC = 0;
	PORTC = 0;


	//set r to "0"
	SET_BIT(DDRC, r);
	CLR_BIT(PORTC, r);


	// set c to "1"
	CLR_BIT(DDRC, c+4);
	SET_BIT(PORTC, c+4);


	avr_wait(1);


	if (!GET_BIT(PINC, c+4)){
		return 1;
	}
	return 0;
}


int get_key()
{
	int i, j;
	for (i = 0; i < 4; ++i)
	{
		for (j = 0; j < 4; ++j){
			if (is_pressed(i, j)){
				return i*4 + j + 1;
			}
		}
	}
	return 0;
}

void print_sample(const int newSample) {
	lcd_clr();
	char buf1[16];
	lcd_pos(0, 0);
	sprintf(buf1, "%.2fV %.2fV", ((double)sum / (double)count) / 1023.0 * 5.0, (newSample / 1023.0) * 5);
	lcd_puts2(buf1);

	char buf2[16];
	lcd_pos(1, 0);
	sprintf(buf2, "%.2fV %.2fV", (max / 1023.0) * 5, (min / 1023.0) * 5);
	lcd_puts2(buf2);
}

void print_diff_sample(signed int newSample) {
	lcd_clr();
	char buf1[16];
	lcd_pos(0, 0);
	sprintf(buf1, "%.2fV %.2fV", ((double)diff_sum / (double)count) / 511.0 * 5.0, ((double)newSample / 511.0) * 5);
	lcd_puts2(buf1);

	char buf2[16];
	lcd_pos(1, 0);
	sprintf(buf2, "%.2fV %.2fV", (diff_max / 511.0) * 5, (diff_min / 511.0) * 5);
	lcd_puts2(buf2);
}

int get_sample() {
	// configure the ADC
	ADMUX = 0b01000000;
	ADCSRA = 0b11000000;

	// start conversion
	while (GET_BIT(ADCSRA, 6)); // bit 6 of ADCSRA

	// wait for conversion result
	return ADC;
}

signed int get_differential_sample() {
	ADMUX = 0b01010000;

	ADCSRA = 0b11000000; 

	// Wait for the conversion to finish
	while (GET_BIT(ADCSRA, 6));

	// Read the result
	signed int result = ADC;
	
	if (result & 0x0200) {  
		result |= 0xFC00;  
	}

	return result;
}

void compute_diff_sample(signed int new_sample) {
	if (new_sample > diff_max) {
		diff_max = new_sample;
	}
	if (new_sample < diff_min) {
		diff_min = new_sample;
	}
}

void compute_sample(int new_sample) {
	if (new_sample > max) {
		max = new_sample;
	}
	if (new_sample < min) {
		min = new_sample;
	}
}


int main() {
	// initialize avr && lcd
	avr_init();
	lcd_init();

	// read the keyboard
	while (1) {
		int k = get_key();
		if (k) {
			switch (k) {
				case 1: // Button 1 Enter Normal Sampling Mode
					reset = 0;
					need_differential_sample = 0;
					need_sample = 1;
					current_mode = 0;
					break;
				case 2: // Button 2 Enter Reset Mode
					reset = 1;
					break;
				case 3: // Button 3
					reset = 0;
					need_sample = 0;
					need_differential_sample = 0;
					break;
				case 4: // Button A
					break;
				case 5: // Button 4 Enter Differential Sampling Mode
					reset = 0;
					need_sample = 0;
					need_differential_sample = 1;
					current_mode = 1;
					break;
				case 6: // Button 5
					break;
				case 7: // Button 6
					break;
				case 8: // Button B
					break;
				case 9: // Button 7
					break;
				case 10: // Button 8
					break;
				case 11: // Button 9
					break;
				case 12: // Button C
					break;
				case 13: // Button *
					break;
				case 14: // Button 0
					break;
				case 15: // Button #
					break;
				case 16: // Button D
					break;
				default:
					break;
			}
		}
		
		if(reset) {
			diff_min = 0;
			diff_sum = 0;
			diff_max = 0;
			sum = 0;
			max = 0;
			min = 0;
			count = 1;
			need_sample = 0;
			need_differential_sample = 0;
			if(!current_mode) 
			{
				print_sample(get_sample());
				
			}
			else
			{
				print_diff_sample(get_differential_sample());
			}
			count = 0;
			avr_wait(50);
		}

		if (need_sample) {
			int new_sample = get_sample();
			++count;
			sum += new_sample;
			compute_sample(new_sample);
			print_sample(new_sample);
			avr_wait(500);
		}
		if (need_differential_sample)
		{
			signed int new_diff_sample = get_differential_sample();
			++count;
			diff_sum += new_diff_sample;
			compute_diff_sample(new_diff_sample);
			print_diff_sample(new_diff_sample);
			avr_wait(500); 
		}
	}
	return 0;
}




