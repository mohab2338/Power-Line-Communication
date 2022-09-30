/*
 * main.c
 *
 *  Created on: Dec 14, 2021
 *      Author: Omar
 */

#define F_CPU 8000000UL
#include "util/delay.h"

#include "std_types.h"
#include "BIT_MATH.h"

#include "Dio_interface.h"
#include "EXTI_interface.h"
#include "Timer_interface.h"
#include "Timer1_interface.h"
void ZeroCrossing();
void Display(void);
void Pulse(u8 Copy_BitValue);
void ZCD(void);
#define DATAWIDTH  12
#define READTIME	250
#define WRITETIME  1000

#define INPUTPORT				DIO_u8PORT_D
#define INPUTPIN				DIO_u8PIN_0

#define DISPLAYPORT_2			DIO_u8PORT_B
#define DISPLAYPINVALUE_2		   0x9f
#define DISPLAYPORT_1			DIO_u8PORT_C
#define DISPLAYPINVALUE_1			0xff

#define ADDRESSPORT				DIO_u8PORT_A
#define ADDRESSPINS					0x00

#define TRANSMITPORT			DIO_u8PORT_D
#define TRANSMITPIN				DIO_u8PIN_5

static u8 display[DATAWIDTH] = {0,0,0,0 ,0,0,0,0 ,0,0,0,0 };
static u8 CrossZero = 0;
static u8 on_off = 0; // state of device controlled by the receiver
static u8 start = 0;
static u8 count = 0;
int main(void)
{	DIO_voidSetPinDirection(DIO_u8PORT_D,DIO_u8PIN_1,DIO_u8OUTPUT);//zero crossing indicator
	DIO_voidSetPortDirection(ADDRESSPORT,ADDRESSPINS);//defining address pins
	DIO_voidSetPinDirection(INPUTPORT,INPUTPIN,DIO_u8INPUT); // input Data  pin
	DIO_voidSetPortDirection(DISPLAYPORT_1,DISPLAYPINVALUE_1);// Display 1
	DIO_voidSetPortDirection(DISPLAYPORT_2,DISPLAYPINVALUE_2);// Display and device on_off
	DIO_voidSetPinDirection(TRANSMITPORT,TRANSMITPIN,DIO_u8OUTPUT);//transmit pin OC1A
	/*Enable external interrupt for zeroCrossing detection*/
	EXTI_u8INT0SetCallback(ZeroCrossing);
    EXTI0_voidInit(EXTI_RISING_EDGE);
    /*Enable Timer1 */
	Timer1_voidInit();
	DIO_voidSetPinDirection(DIO_u8PORT_D,DIO_u8PIN_7,DIO_u8OUTPUT);//test for reading only
	while(1){
	}
}// End main

void ZeroCrossing(){
	static u8 address = 0;
	_delay_us(READTIME);
	/*Read Pulse for indication*/
	DIO_voidSetPinValue(DIO_u8PORT_D,DIO_u8PIN_7,DIO_u8HIGH);
	_delay_us(100);
	DIO_voidSetPinValue(DIO_u8PORT_D,DIO_u8PIN_7,DIO_u8LOW);
	/**************************/
	display[CrossZero] = DIO_u8GetPinValue(INPUTPORT,INPUTPIN);

	if( (display[CrossZero]==1)&&(count<4)    ){
		count++;
		CrossZero++;
	}
	else if(count==4){
		//display[CrossZero] = DIO_u8GetPinValue(DIO_u8PORT_A,DIO_u8PIN_0);
		//DIO_voidSetPinValue(DIO_u8PORT_B,DIO_u8PIN_5,display[CrossZero]);//bit test
		CrossZero++;

		address = display[4]*1+display[5]*2+display[6]*4+display[7]*8;
		if( (CrossZero >= DATAWIDTH)&&(address == DIO_u8GetPortValue(DIO_u8PORT_A))  ){
			Display();
			while(DIO_u8GetPinValue(DIO_u8PORT_D,DIO_u8PIN_2) == 1)
			{
				//wait till falling edge then send pulse
			}
			Pulse(1);
		}

		else if(CrossZero >= DATAWIDTH){
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_0,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_1,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_2,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_3,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_4,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_5,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_6,DIO_u8LOW);
			DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_7,DIO_u8LOW);
			DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_0,DIO_u8LOW);
			DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_1,DIO_u8LOW);
			DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_2,DIO_u8LOW);
			DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_3,DIO_u8LOW);

			CrossZero = 0;
			//start = 0;
			count = 0;

			for(u8 count=0;count<DATAWIDTH;count++){
				display[count]=0;
			}
		}
	}
	else{
		count = 0;
		CrossZero = 0;
	}




}//End zero crossing

void Display(void){
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_0,display[0]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_1,display[1]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_2,display[2]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_3,display[3]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_4,display[4]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_5,display[5]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_6,display[6]);
	DIO_voidSetPinValue(DIO_u8PORT_C,DIO_u8PIN_7,display[7]);

	DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_0,display[8]);
	DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_1,display[9]);
	DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_2,display[10]);
	DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_3,display[11]);
	if( on_off == 0 ){
		DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_4,DIO_u8HIGH);
		on_off = 1;
	}
	else if( on_off == 1 ){
		DIO_voidSetPinValue(DISPLAYPORT_2,DIO_u8PIN_4,DIO_u8LOW);
		on_off = 0;
	}
	//while(DIO_u8GetPinValue(DIO_u8PORT_D,DIO_u8PIN_2) == 1);
	//Pulse(1);
	CrossZero = 0;
	start = 0;
	count = 0;
	for(u8 count=0;count<DATAWIDTH;count++){
		display[count]= 0;
	}
}//End Display
void Pulse(u8 Copy_BitValue){

	Timer1_SetCompareMatchA_Value((8000000/(2*129000))-1);
	Timer1PWM_A_OC1A( Copy_BitValue );
	_delay_us(WRITETIME);
	Timer1PWM_A_OC1A( 0 );
}//End Pulse
void ZCD(void)
{
	static u8 state=0;
	if(state == 0)
	{
	DIO_voidSetPinValue(DIO_u8PORT_B,DIO_u8PIN_7,DIO_u8HIGH);
	state = 1;
	}
	else
	{
	DIO_voidSetPinValue(DIO_u8PORT_B,DIO_u8PIN_7,DIO_u8LOW);
	state =0;
	}

}
