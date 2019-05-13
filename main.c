
#include<avr/io.h>
#include<util/delay.h>
#include "sbit.h"
#include "lcd.h"
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define DT_OUT 	SBIT(PORTC,4)
#define DT_IN 	SBIT(PINC,4)
#define DT_DIR	SBIT(DDRC,4)

#define SCK			SBIT(PORTC,5)
#define SCK_DIR		SBIT(DDRC,5)

#define FLOOR1_SW	SBIT(PINB,0)
#define FLOOR2_SW	SBIT(PINB,5)

#define FLOOR1_LIFT	SBIT(PINB,4)
#define FLOOR2_LIFT	SBIT(PINB,3)

#define FLOOR1_IR	SBIT(PINB,1)
#define FLOOR2_IR	SBIT(PINB,2)

#define M_EN		SBIT(PORTC,3)
#define M_EN_DIR	SBIT(DDRC,3)
#define M_IN1		SBIT(PORTC,2)
#define M_IN1_DIR	SBIT(DDRC,2)
#define M_IN2		SBIT(PORTC,1)
#define M_IN2_DIR	SBIT(DDRC,1)

enum{CHECK_REQ,MOVE_LIFT,FLOOR_REQ,RUN_LIFT};

long sample=0;
float val=0;
long count=0;

int lift_floor;

int lift_moving = 0;

void run_elevator(void);

unsigned long readCount(void)
{
  unsigned long Count;
  unsigned char i;
  DT_DIR = 1;
  DT_OUT = 1;
  SCK = 0;
  Count=0;
  DT_DIR = 0;
  while(DT_IN);
  for (i=0;i<24;i++)
  {
    SCK = 1;
    Count=Count<<1;
    SCK = 0;
    if(DT_IN)
    Count++;
  }
  SCK = 1;
  Count=Count^0x800000;
  SCK = 0;
  return(Count);
}

int main()
{
	DDRB=0x00;
	DDRC=0xff;
	DDRD=0xFF;
	SCK_DIR = 1;
	M_EN_DIR = 1;
	M_IN1_DIR = 1;
	M_IN2_DIR = 1;
	M_EN = 1;
	_delay_ms(100);
	lcd_init();
	lcd_printf("ELEVATOR");
	_delay_ms(2000);
	int w;
	char arr[5];
	lcd_clear();
	while(1)
	{
	  count= readCount();
	  w = (((count-(long)8273988)/54.54)-2*((count-(long)8273988)/54.54))+2000;
	  //w = (((count-sample)/val)-2*((count-sample)/val));
	  w/=2;
	  w-=100;
	  if(w<0)w=0;
	  if(!lift_moving)
	  {
		  if(!FLOOR1_LIFT)
		  {
			  if(FLOOR1_IR)
			  {
				//state = MOVE_LIFT;
			  }
			  else
			  {
				  lcd_gotoxy(2,0);
				  lcd_printf("MOVING DOWN");
				  while(!FLOOR1_IR)
				  {
					  M_IN1 = 1;
					  M_IN2 = 0;
					  lift_moving = 1;
				  }
				  if(lift_moving)
				  {
					  lift_moving = 0;
					  _delay_ms(2000);
				  }
				  M_IN1 = 0;
				  M_IN2 = 0;
				  lcd_gotoxy(2,0);
				  lcd_printf("STOP       ");
			  }
		  }
		  else if(!FLOOR2_LIFT)
		  {
			  if(FLOOR2_IR)
			  {
				//state = MOVE_LIFT;
			  }
			  else
			  {
				  lcd_gotoxy(2,0);
				  lcd_printf("MOVING UP  ");
				  while(!FLOOR2_IR)
				  {
					  M_IN1 = 0;
					  M_IN2 = 1;
					  lift_moving = 1;
				  }
				  if(lift_moving)
				  {
					  lift_moving = 0;
					  _delay_ms(2000);
				  }
				  M_IN1 = 0;
				  M_IN2 = 0;
				  lcd_gotoxy(2,0);
				  lcd_printf("STOP       ");
			  }
		  }
	  }
	  itoa(w,arr,10);
	  lcd_gotoxy(1,0);
	  lcd_printf(arr);
	  lcd_printf("gm     ");
	  if((w >= 500)&&(w<=3000))
	  {
		  lcd_gotoxy(2,0);
		  lcd_printf("UNDERLOAD  ");
		  run_elevator();
	  }
	  else if(w > 3000)
	  {
		  lcd_gotoxy(2,0);
		  lcd_printf("OVERLOAD   ");
	  }
	  else
	  {
		  lcd_gotoxy(2,0);
		  lcd_printf("NO LOAD    ");
	  }
	}
	return(0);
}

void run_elevator(void)
{
	if(!FLOOR1_SW)
	{
		if(FLOOR1_IR){}
		else
		{
			lcd_gotoxy(2,0);
			lcd_printf("MOVING DOWN");
			while(!FLOOR1_IR)
			{
				M_IN1 = 1;
				M_IN2 = 0;
				lift_moving = 1;
			}
			if(lift_moving)
			{
				lift_moving = 0;
				_delay_ms(2000);
			}
			M_IN1 = 0;
			M_IN2 = 0;
			lcd_gotoxy(2,0);
			lcd_printf("STOP       ");
		}
	}
	else if(!FLOOR2_SW)
	{
		if(FLOOR2_IR){}
		else
		{
			lcd_gotoxy(2,0);
			lcd_printf("MOVING UP  ");
			while(!FLOOR2_IR)
			{
				M_IN1 = 0;
				M_IN2 = 1;
				lift_moving = 1;
			}
			if(lift_moving)
			{
				lift_moving = 0;
				_delay_ms(2000);
			}
			M_IN1 = 0;
			M_IN2 = 0;
			lcd_gotoxy(2,0);
			lcd_printf("STOP       ");
		}
	}
}
