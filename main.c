#include<main.h>
#include "port.h"
#include <INTRINS.H>

#define sda			P0_6
#define scl			P0_7
#define remote 		P3_2

#define max_start_0 		0xcd;// 20 ms  chieu rong toi da bit start
#define max_start_1 		0xa3;// max_start = 0xffff - 20000 = 0xb1df
#define min_start    		0x733;// 2 ms  chieu rong toi thieu bit start
#define max_data_0  		0x41;// 3  ms	chieu rong toi da bit data
#define max_data_1 		  0xe1;// max_data = 0xff - 3000 = 0xf447 

unsigned char e = 0, r = 0, a, b;
unsigned char count2 = 0, step = 0, number_code = 0, dem = 0;
unsigned long idata data_, c, d; // du lieu giai ma
unsigned char led7[]  = {0xC0,0xF3,0xA4,0xA1,0x93,0x89,0x88,0xe3,0x80,0x81,0x82,0x98,0xCC,0xB0,0x8C,0x8E}; //led 7 doan

//=================== delay_ms ===================
void delay(unsigned char time)
{
	while(time--){ 
		unsigned char temp = 121;     // 1 vòng lap ton 8 us      
		while(temp--);  
		}
}




//======================Tat=======================
void q_off()
{

	led2 = led3 = 1;
	P1 = 0xff;
	led2 = 0;
	P1=(0xc0);
	delay(3);
	
	led2 = led3 = 1;
	P1 = 0xff;
	led3 = 0;
	P1=(0x8e);
	delay(3);
}

//=================Khoi tao Ngat==================
void khoitaongat() 
{
	TMOD = 0x11; 	// Chon Timer 1 che do 1 (16bit) va Timer 0 che do 1 (16bit)
	
	EX0  = 1;	   	// Cho phep ngat ngoai 0
	IT0  = 1;		 	// Ngat ngoai 0 theo suon am
	IE0  = 0;
	
	ET0  = 1;		  // Cho ngat Timer0	
	
	IT1  = 1;		 	// Ngat ngoai 1 theo suon am
	EX1  = 1;	   	// Cho phep ngat ngoai 1
	IE1  = 0;
	
	PT0  = 1; 		// Cho phep ngat ngoai uu tien hon ngat trong
	EA   = 1;	    // Cho phep ngat

	ET1  = 1;	   	// Cho phep ngat Timer 1
	TH1  = 0x4b;
	TL1  = 0xfd;	// Nap gia tri cho Timer 1, 50ms
	TR1  = 1;			// Khoi dong Timer 1
	
	sda=1;
	scl=1;
	remote=1;
}

/**********************************************************************************/
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 //                    CAC CHUONG TRINH CON GIAI MA REMOTE                   //
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
void ngat_0(void) interrupt 0		//ngat ngoai 0 ( remote )
{ 
	unsigned short do_rong_xung, min;
	
	TR0 = 0;// stop timer 0
	if(step==0)// bit start
	{
		step = 1;
		data_=0;
		TL0 = max_start_0;
		TH0 = max_start_1;
		TR0 = 1;// khoi dong timer 0	
	}
	else if(step==1)// bit data
	{
		do_rong_xung = TH0-max_start_1;
		do_rong_xung = (do_rong_xung<<8) + TL0-max_start_0;
		min = min_start;
		if(do_rong_xung > min)
		{
			step = 2;
			TL0 = max_data_0;
			TH0 = max_data_1;
			TR0 = 1;// khoi dong timer 0	
		}
		else step = 0;
	}
	else if(step==2)// bit data
	{	  
		do_rong_xung = TH0 - max_data_1 ;
		do_rong_xung = (do_rong_xung<<8) + TL0- max_data_0;
		TL0 = max_data_0;
		TH0 = max_data_1;
		TR0 = 1;// khoi dong timer 0
		data_=data_<<1; //  bit 0
		if (do_rong_xung >	0x5DC) data_= data_+1;// neu do rong lon hong 1500us thi la bit 1	
	}
}

//================= ngat Timer 0 =================
void timer_0(void) interrupt 1		//ngat timer 0 de giai ma 
{ 
	TR0 =0;				//stop timer 0
	if (step==1) 
		step = 0;	// giai ma that bai, bit start qua dai	
	else if (step==2)		// Bit stop
	{
		EX0 = 0; 			// cam ngat ngoai 0 de xu ly ket qua giai ma
		step = 3;
	}
}

//=============ngat ngoai 1=======================
void on() interrupt 2	//ngat chan P3.2 (INT0)
{
	if(count2 >= 5)
	{
		dem++;
		if(dem >= 4)
			dem = 0;
		count2 = 0;
	}
}

//=============ngat Timer 1 (hen gio)=============
void hengio() interrupt 3
{
	TH1 = 0x4b;
	TL1 = 0xfd;	// nap gia tri cho timer, 50ms
	
	if (EX0 == 0)
	{
		e++;
		if(e >= 5)
		{				
			EX0 = 1;
			e = 0;
		}
	}
	if(count2 < 7)
		count2++;
}

//===============Chuong trinh chinh===============
void main()
{	
	khoitaongat();	
	sp = 0;
	delay(50);
	sp = 1;
	q_off();
	data_ = 0;
	while(1)
	{
		if(step == 3)
		{
			if(dem == 0)
			{
				a = data_ >> 28;
				d = data_ << 4;
				b = d >> 28;
				
				led2 = led3 = 1;
				P1 = 0xff;
				led2 = 0;	
				P1=led7[a];
				delay(3);
				
				led2 = led3 = 1;
				P1 = 0xff;
				led3 = 0;
				P1=led7[b];
				delay(3);
			}
			
			else if(dem == 1)
			{
				c = data_ << 8;
				a = c >> 28;
				d = data_ << 12;
				b = d >> 28;
				
				led2 = led3 = 1;
				P1 = 0xff;
				led2 = 0;	
				P1=led7[a];
				delay(3);
				
				led2 = led3 = 1;
				P1 = 0xff;
				led3 = 0;
				P1=led7[b];
				delay(3);
			}
			
			if(dem == 2)
			{
				c = data_ << 16;
				a = c >> 28;
				d = data_ << 20;
				b = d >> 28;
				
				led2 = led3 = 1;
				P1 = 0xff;
				led2 = 0;	
				P1=led7[a];
				delay(3);
				
				led2 = led3 = 1;
				P1 = 0xff;
				led3 = 0;
				P1=led7[b];
				delay(3);
			}
			
			if(dem == 3)
			{
				c = data_ << 24;
				a = c >> 28;
				d = data_ << 28;
				b = d >> 28;

				
				led2 = led3 = 1;
				P1 = 0xff;
				led2 = 0;	
				P1=led7[a];
				delay(3);
				
				led2 = led3 = 1;
				P1 = 0xff;
				led3 = 0;
				P1=led7[b];
				delay(3);
			}
		}
	}
}


