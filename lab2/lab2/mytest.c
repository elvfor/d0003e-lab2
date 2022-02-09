/*
 * mytest.c
 *
 * 
 *  Author: Elvira Forslund Widenroth & Jenny Sundström
 */ 

#include "tinythreads.h"
#include <stdbool.h>
#include <avr/io.h>
#include <stdint-gcc.h>
#include <avr/portpins.h>
#include <stdlib.h>

int pp;
mutex m = MUTEX_INIT;

int numbers [10] = {
	0x1551,					// 0
	0x0110,					// 1
	0x1e11,					// 2
	0x1b11,					// 3
	0x0b50,					// 4
	0x1b41,					// 5
	0x1f41,					// 6
	0x0111,					// 7
	0x1f51,					// 8
	0x1b51,					// 9
};

void LCD_Init(void){
	/* Use 32 kHz crystal oscillator */
	/* 1/3 Bias and 1/4 duty, SEG0:SEG24 is used as port pins, external asynchronous clock source is used */
	LCDCRB = (1<<LCDCS) | (1<<LCDMUX1)| (1<<LCDMUX0)| (1<<LCDPM2) |(1<<LCDPM1) |(1<<LCDPM0) | (1<<LCDCS);
	
	/* Using 16 as pre-scaler selection and 8 as LCD Clock Divide */
	/* gives a frame rate of 49 Hz */
	LCDFRR = (1<<LCDCD2) | (1<<LCDCD1) | (1<<LCDCD0);
	
	/* Set segment drive time to 300s and output voltage to 3.35 V*/
	LCDCCR = (0<<LCDDC0) | (0<<LCDDC1) | (0<<LCDDC2) | (1<<LCDCC3) | (1<<LCDCC0) | (1<<LCDCC1) | (1<<LCDCC2);
	
	/* Enable LCD, low power waveform and no frame interrupt enabled */
	LCDCRA = (1<<LCDEN) | (1<<LCDAB);
}
void writeChar(char ch, int pos){ // from lab 1
	
	int i = (ch - '0');
	char *LCDDRX;				//creates pointer
	LCDDRX = &LCDDR0;			//start pointer at LCDDR0
	char bitmask;
	int shift;
	int number = 0;

	
	if ((pos >= 0) && (pos <= 5)){
		
		int number = numbers[(int)i];			//retrieves which char to print from array
		
		if (pos % 2 == 0) {						//if position is even no shift is made
			shift = 0;
			bitmask = 0xF0;						//saves 4 MSB of register
			
			}else{
			shift = 4;							//if position is odd shift 4
			bitmask = 0x0F;						//saves 4 LSB of register
		}
		
		
		//sets pointer to correct register
		LCDDRX += (pos / 2);
		
		
		//for setting the four nibbles to corresponding character
		for(int i = 0; i < 4; i++) {
			*LCDDRX = ((*LCDDRX & bitmask) | (number & 0x0F) << shift);			// mask register or 4 LSB of character shifted accordingly
			number = (number >> 4);												// remove 4 LSB of character
			LCDDRX += 5;														// set pointer to next register
		}
	}
}

//bool is_prime(long i); // from lab 1
int is_prime(long i){
	int n = 2;
	while(n<i){
		if ((i % n) == 0){
			return 0;
			}else{
			n++;
		}
		return 1;
	}
}

void printAt(long num, int pos) {
	lock(&m);
    pp = pos;
    writeChar( (num % 100) / 10 + '0', pp);
    pp++;
    writeChar( num % 10 + '0', pp);
	unlock(&m);
}

void computePrimes(int pos) {
    long n;

    for(n = 1; ; n++) {
        if (is_prime(n)) {
            printAt(n, pos);
			//yield();
        }
    }
}

int main() {
	CLKPR = 0x80;
	CLKPR = 0x00;
	LCD_Init();
    spawn(computePrimes, 0);
    computePrimes(3);
	//LCDDR2 = 0xffff;
}
