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
	int bit1 = DIGITALIO_Read(ARDUINO_BIT_1);
	int bit2 = DIGITALIO_Read(ARDUINO_BIT_2);
	int bit3 = DIGITALIO_Read(ARDUINO_BIT_3);

	return (bit3 * 4 + 2 * bit2 + bit1);

}
