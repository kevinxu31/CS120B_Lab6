/*	Author: lab
 *  Partner(s) Name: Luofeng Xu
 *	Lab Section:022
 *	Assignment: Lab 6  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:Youtube URL>https://youtu.be/xxOUrS4bZgs
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag=0;
unsigned long _avr_timer_M=1;
unsigned long _avr_timer_cntcurr=0;
void TimerOn(){
	TCCR1B=0x0B;
	OCR1A=125;
	TIMSK1=0x02;
	TCNT1=0;
	_avr_timer_cntcurr=_avr_timer_M;
	SREG |=0x80;
}
void TimerOff(){
	TCCR1B=0x00;
}
void TimerISR(){
	TimerFlag=1;
}
ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr==0){
		TimerISR();
		_avr_timer_cntcurr=_avr_timer_M;
	}
}
void TimerSet(unsigned long M){
	_avr_timer_M=M;
	_avr_timer_cntcurr=_avr_timer_M;
}

enum States{Start,Blink,Pause}state;

unsigned char b;
unsigned char t;
unsigned char A0;
void Tick(){
	switch(state){
		case Start:
			t=0;
			b=0x01;
			state=Blink;
			break;
		case Blink:
			if(t==4){t=0;}
			if(!A0){
				state=Blink;
			}
			if(A0){
				state=Pause;
			}
			break;
		case Pause:
			if(!A0){
				state=Pause;
			}
			else if (A0){
				state=Start;
			}
		default:
			break;
	
	}
	switch(state){
		case Start:
			break;
		case Blink:
			if (t==0){
				b=0x01;
			}
			else if (t>0&&t<3){
				b=b<<1;
			}
			else if(t>=3){
				b=b>>1;
			}
			t++;
			PORTB=b;
			break;
		case Pause:
			PORTB=b;
			break;
	}


}




void main() {
	DDRA=0x00;PORTA=0xFF;
	DDRB=0xFF;PORTB=0x00;
	state=Start;
	TimerSet(300);
	TimerOn();
	while (1) {
		A0=~PINA&0x01;
		Tick();
		while(!TimerFlag){};
		TimerFlag=0;
	}
}
