#ifndef _PORT_H_
#define _PORT_H_

//dieu khien quat
sbit so1  	= P2^3;
sbit so2  	= P2^2;
sbit so3  	= P2^0;
sbit quay 	= P2^1;

//dieu khien led 7 doan
sbit led2 	= P3^0;
sbit led3 	= P3^1;
sbit sp   	= P3^5;
sbit den 		= P2^5;

//nut nhan
sbit btquay = P0^2;
sbit on_off = P0^5;
sbit bden   = P0^4;

//select
sbit se     = P0^0;

#endif