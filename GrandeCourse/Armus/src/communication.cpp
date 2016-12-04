/*
 * communication.cpp
 *
 *  Created on: 2016-12-03
 *      Author: boug2629
 */

#include <libarmus.h>
#include "communication.h"
#include "constante.h"


void BTsendState(int state)
{
	int bit[3];
	int i =0;
	while (i < 3)
	{
		bit[i] = state%2;
		state /= 2;
		i++;
	}


	DIGITALIO_Write(BIT1,bit[0]);
	DIGITALIO_Write(BIT2,bit[1]);
	DIGITALIO_Write(BIT3,bit[2]);
}

int BTreadState(void)
{
	//float comm = ANALOG_Read(ARDUINO);
	float comm = 1023;
	comm /= 1023/8.0 ;
	float reste = comm - (int)comm;
	if(reste >= 0.5)
		comm = (int)comm +1;
	else
		comm = (int)comm;
	//manque conversion en deca

	return comm;
}
