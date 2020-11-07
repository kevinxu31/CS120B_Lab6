/*	Author: lab
 *  Partner(s) Name: Luofeng Xu
 *	Lab Section:022
 *	Assignment: Lab 6  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link:Youtube URL>https://youtu.be/ffTaZFUCbrw
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



enum States{Start,RA,PA_0,Inc,Inc_a,PA_1,Dec,Dec_a,Reset}state;
unsigned char A0;
unsigned char A1;
unsigned char B;
unsigned char t;
void Tick(){
	switch(state){
		case Start:
			B=7;
			t=0;
			state=RA;
			break;
		case RA:
			t=0;
			if(A0&(!A1)){
				state=PA_0;
				if(B<9){B++;}

			}
			else if((!A0)&A1){
				state=PA_1;
				if(B>0){B--;}
			}
			else if(A0&A1){
				state=Reset;
			}
			break;
		case PA_0:
			if (A0&(!A1)&(t<10)){
				state=PA_0;
			}
			else if((!A0)&(!A1)){
				state=RA;
			}
			else if(A0&A1){
				state=Reset;
				B=0;
			}
			else if(A0&(!A1)&(t>=10)){
				state=Inc;
				t=0;
			}
			break;
		case Inc:
			if(A0&(!A1)){
				state=Inc;
				if(t<=20){
					if((B<9)&(t%5==0)){B++;}
				}
				else if(t>20){
					if((B<9)&(t%2==0)){B++;}
				}
			
			}
			else if((!A0)&(!A1)){
				state=RA;
			}
			else if(A0&A1){
				state=Reset;
				B=0;
			}
			break;
                case PA_1:
			if (A1&(!A0)&(t<10)){
                                state=PA_1;
                        }
			else if(A1&(!A0)&(t>=10)){
				state=Dec;
				t=0;
			}
                        else if((!A0)&(!A1)){
                                state=RA;
                        }
                        else if(A0&A1){
                                state=Reset;
                                B=0;
                        }
                        break;
		case Dec:
                        if(A1&(!A0)){
                                state=Dec;
				if(t<=20){
					if((B>0)&(t%5==0)){B--;}
				}
				else if(t>20){
					if((B>0)&(t%2==0)){B--;}
				}
                        }
                        else if((!A0)&(!A1)){
                                state=RA;
                        }
                        else if(A0&A1){
                                state=Reset;
                        }
                        break;

		case Reset:
			if((!A1)&(!A0)){state=RA;}
			else{state=Reset;}
			break;
		default:
			break;
	}
	switch(state){
		case Start:
			break;
		case RA:
			t=0;
			break;
		case PA_0:
			t++;
			break;
		case PA_1:
			t++;
			break;
		case Inc:
			t++;
			break;
		case Dec:
			t++;
			break;
		case Reset:
			B=0;
			break;
		default:
			break;
	}



}

int main(void) {
	DDRA=0x00;PORTA=0xFF;
	DDRB=0xFF;PORTB=0x00;
	TimerSet(100);
	TimerOn();
	state=Start;
	while (1) {
		A0=~PINA&0x01;
		A1=(~PINA>>1)&0x01;
		Tick();
		while(!TimerFlag){};
		TimerFlag=0;
		PORTB=B;
	}
	return 1;
}
