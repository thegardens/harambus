/*
 * capteurcouleur.cpp
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */
#include "capteurcouleur.h"
#include <libarmus.h>

unsigned int countCycleOsc=0;
int countTest=0;
bool flagFisrtPass=true;
int lastColor=0;
int adjd_dev;

/*--------------Fonctions pour les capteurs-----------------------------------------------------------------------*/
//Capteurs de couleurs
void adjd_SetRegister(unsigned char reg, unsigned char val)
{
	unsigned char data[2];
	data[0] = reg;
	data[1] = val;
	armus::i2c_Write(adjd_dev, 2, data);
}

//permet de changer la valeur des registres de 16 bits
void adjd_SetRegister16(unsigned char reg, int val)
{
	unsigned char data[2];
	data[0] = reg;
	data[1] = val & 0xFF;
	armus::i2c_Write(adjd_dev, 2, data);
	data[0] = reg+1;
	data[1] = (val >> 8) & 0xFF;
	armus::i2c_Write(adjd_dev, 2, data);
}

//permet de lire la valeur des registres
unsigned char adjd_ReadRegister(unsigned char reg)
{
	unsigned char val;

	armus::i2c_ReadAfterWrite(adjd_dev, 1, &reg, 1, &val);

	return val;
}

//permet de lire la valeur des registres de 16 bits
int adjd_ReadRegister16(unsigned char reg)
{
	int val16;
	unsigned char val;
	armus::i2c_ReadAfterWrite(adjd_dev, 1, &reg, 1, &val);
	val16 = val;
	reg = reg+1;
	armus::i2c_ReadAfterWrite(adjd_dev, 1, &reg, 1, &val);
	val16 = val16 + ((val << 8) & 0xFF00);
	return val16;
}


// Permet de connaitre la valeur du CAP dans le registre
// prend comme argument une constante CAP_RED, CAP_BLUE, CAP_CLEAR ou CAP_GREEN
// retourne un unsigned char de la valeur
unsigned char cap_GetValue(unsigned char cap_address)
{
	unsigned char cap_value;

	cap_value = adjd_ReadRegister(cap_address);

	return cap_value;
}


// Permet de changer la valeur du CAP dans le registre
// prend comme premier argument une constante CAP_RED, CAP_BLUE, CAP_CLEAR ou CAP_GREEN
// le second argument est compris entre 0 et 15, et determine la valeur du cap a ecrire dans le registre
void cap_SetValue(unsigned char cap_address, unsigned char cap_value)
{
	adjd_SetRegister(cap_address, cap_value);
}



// Permet de connaitre la valeur du CAP dans le registre
// address est une constante comme INTEGRATION_RED, ...
// retourne un int de la valeur
int integrationTime_GetValue(unsigned char address)
{
	int time_value;

	time_value = adjd_ReadRegister16(address);

	return time_value;
}


// Permet de changer la valeur du CAP dans le registre
// address est une constante comme INTEGRATION_RED, ...
// time_value est compris entre 0 et 4095
void integrationTime_SetValue(unsigned char address, int time_value)
{
	adjd_SetRegister16(address, time_value);
}


// Vous devez vous-meme modifier cette fonction tout dependamment de la sortie numerique utilisee
void led_TurnOff()
{
	// TODO : code a changer
	DIGITALIO_Write(9, 0);
}

// Vous devez vous-meme modifier cette fonction tout dependamment de la sortie numerique utilisee
void led_TurnOn()
{
	// TODO : code a changer
	DIGITALIO_Write(9, 1);
}

// permet de faire une lecture differentielle avec et sans eclairage de la led
void color_Read(int& data_red, int& data_blue, int& data_green, int& data_clear)
{
	//premiere lecture sans eclairage
	led_TurnOff();

	adjd_SetRegister(ADJD_REG_CONFIG, 1 << CONFIG_TOFS);
	adjd_SetRegister(ADJD_REG_CTRL, 1 << CTRL_GOFS);
	while(adjd_ReadRegister(ADJD_REG_CTRL))
	{
		THREAD_MSleep(50);
	}

	//lecture avec eclairage
	led_TurnOn();
	adjd_SetRegister(ADJD_REG_CTRL, 1 << CTRL_GSSR);
	while(adjd_ReadRegister(ADJD_REG_CTRL))
	{
		THREAD_MSleep(50);
	}

	//eteindre la led
	led_TurnOff();

	data_red = adjd_ReadRegister16(DATA_RED_LO);
	data_green = adjd_ReadRegister16(DATA_GREEN_LO);
	data_blue = adjd_ReadRegister16(DATA_BLUE_LO);
	data_clear = adjd_ReadRegister16(DATA_CLEAR_LO);
}

void color_ReadToCalibrate(int& data_red, int& data_blue, int& data_green, int& data_clear)
{
	led_TurnOn();
	adjd_SetRegister(ADJD_REG_CONFIG, 0 << CONFIG_TOFS);
	adjd_SetRegister(ADJD_REG_CTRL, 1 << CTRL_GSSR);
	while(adjd_ReadRegister(ADJD_REG_CTRL))
	{
		THREAD_MSleep(50);
	}
	led_TurnOff();

	data_red = adjd_ReadRegister16(DATA_RED_LO);
	data_green = adjd_ReadRegister16(DATA_GREEN_LO);
	data_blue = adjd_ReadRegister16(DATA_BLUE_LO);
	data_clear = adjd_ReadRegister16(DATA_CLEAR_LO);

}

// l argument est un integer qui ne doit pas etre modifie
int color_Init(int& dev_handle)
{
	int error;
	error = armus::i2c_RegisterDevice(ADJD_S371_QR999_SADR, 100000, 1000, dev_handle);

	return error;
}


void osc_couleur(int couleur)
{
	int motorControl=35;

	switch (couleur)
	{
	case 1: if(lastColor!=couleur)
				countCycleOsc=0;
			if(countCycleOsc<5)
			{
			MOTOR_SetSpeed(MOTOR_RIGHT,-40);
			MOTOR_SetSpeed(MOTOR_LEFT,-35);

			}
			else if(countCycleOsc>=5 && countCycleOsc<=10)
			{

				motorControl-=(countCycleOsc-5);
				MOTOR_SetSpeed(MOTOR_RIGHT,-40);
				MOTOR_SetSpeed(MOTOR_LEFT,(motorControl));
			}
			else if(countCycleOsc>10)
			{
				MOTOR_SetSpeed(MOTOR_RIGHT,-50);
				MOTOR_SetSpeed(MOTOR_LEFT,-50);
			}
	break;

	case 2: if(lastColor!=couleur)
				countCycleOsc=0;
			if(countCycleOsc<5)
			{
			MOTOR_SetSpeed(MOTOR_RIGHT,-40);
			MOTOR_SetSpeed(MOTOR_LEFT,-35);
			}
			else if(countCycleOsc>=5 && countCycleOsc<=10)
			{
				motorControl-=(countCycleOsc-5);
				MOTOR_SetSpeed(MOTOR_RIGHT,-40);
				MOTOR_SetSpeed(MOTOR_LEFT,(motorControl));
			}
			else if(countCycleOsc>10)
			{
				MOTOR_SetSpeed(MOTOR_RIGHT,-50);
				MOTOR_SetSpeed(MOTOR_LEFT,-50);
			}
	break;

	case 3:  if(lastColor!=couleur)
				countCycleOsc=0;
			if(countCycleOsc<5)
			{
			MOTOR_SetSpeed(MOTOR_RIGHT,-40);
			MOTOR_SetSpeed(MOTOR_LEFT,-35);
			}
			else if(countCycleOsc>=5 && countCycleOsc<=10)
			{
				motorControl-=(countCycleOsc-5);
				MOTOR_SetSpeed(MOTOR_RIGHT,motorControl);
				MOTOR_SetSpeed(MOTOR_LEFT,(-40));
			}
			else if(countCycleOsc>10)
			{
				MOTOR_SetSpeed(MOTOR_RIGHT,-50);
				MOTOR_SetSpeed(MOTOR_LEFT,-50);
			}
	break;

	case 4: if(lastColor!=couleur)
				countCycleOsc=0;
			if(countCycleOsc<5)
			{
			MOTOR_SetSpeed(MOTOR_RIGHT,-40);
			MOTOR_SetSpeed(MOTOR_LEFT,-35);
			}
			else if(countCycleOsc>=5 && countCycleOsc<=10)
			{
				motorControl-=(countCycleOsc-5);
				MOTOR_SetSpeed(MOTOR_RIGHT,motorControl);
				MOTOR_SetSpeed(MOTOR_LEFT,(-40));
			}
			else if(countCycleOsc>10)
			{
				MOTOR_SetSpeed(MOTOR_RIGHT,-50);
				MOTOR_SetSpeed(MOTOR_LEFT,-50);
			}
	break;
	default: MOTOR_SetSpeed(MOTOR_RIGHT,-50);
			 MOTOR_SetSpeed(MOTOR_LEFT,-50);
	break;

	}
	countCycleOsc++;
	lastColor = couleur;

}

int testCouleur()
{

	int red, blue, green, clear, fin;
	float hue;

			//initialisation du capteur
			ERROR_CHECK(color_Init(adjd_dev));

			cap_SetValue(CAP_RED, 0);
			cap_SetValue(CAP_GREEN, 0);
			cap_SetValue(CAP_BLUE, 0);
			cap_SetValue(CAP_CLEAR, 0);

			integrationTime_SetValue(INTEGRATION_RED, 255);
			integrationTime_SetValue(INTEGRATION_GREEN, 255);
			integrationTime_SetValue(INTEGRATION_BLUE, 255);
			integrationTime_SetValue(INTEGRATION_CLEAR, 255);



				color_Read(red, blue, green, clear);
				LCD_ClearAndPrint("R=%d, G=%d, B=%d, C=%d,HUE=%f\n\n", red, green, blue, clear, hue);
				//THREAD_MSleep(50);
				fin==1;


				float R = red/255.0;
				float G = green/255.0;
				float B = blue/255.0;
				int dominantColor= 0;//1 is red , 2 is green , 3 is blue , 0 is default

				if(R>B && R>G)
					dominantColor=1;
				else if(G>R && G>B)
					dominantColor=2;
				else if(B>R && B>G)
					dominantColor=3;
				else
					dominantColor=0;

				switch (dominantColor)
				{
				case 1: if(G>B)
							hue = (G-B)/(R-B);
						else if(B>G)
							hue = (G-B)/(R-G);
				break;

				case 2: if(R>B)
							hue = 2.0 + (B-R)/(G-B);
						else if(B>R)
							hue = 2.0 + (B-R)/(G-R);
				break;

				case 3: if(R>G)
							hue = 4.0 + (R-G)/(B-G);
						else if(G>R)
							hue = 4.0 + (R-G)/(B-R);
				break;

				default: hue=0.0;
				break;


				}
				if(hue>0.0)
					hue = hue*60.0;
				else
				{
					hue=hue*60;
					hue =hue + 360;
				}



				/*while(fin==1);
				{
					if (red>green && green>blue)
						{
							if (red>100 && green>100 )
										{
										LCD_Printf("coubleur jaune\n");
										}
									else LCD_Printf("coubleur rouge\n");
								}



					else if (blue>green && blue>red)
				{
					LCD_Printf("coubleur bleu\n");
				}

				else if (green>blue && green>red)

							{

								LCD_Printf("coubleur verte\n");
							}
				else LCD_Printf("coubleur introuvable\n");
				}*/

				fin==0;
				int hueColor=0;//1 is red , 2 is green , 3 is blue , 4 is yelow, 5 is white, 6 is unknow

				if (hue>=200 && hue <=230)
				{
					hueColor=3;
				}
				else if (hue>=100 && hue <=135)
				{
					hueColor=2;
				}
				else if (hue>=10 && hue <=25)
				{
					hueColor=1;
				}
				else if (hue>=40 && hue <=57)
				{
					hueColor=4;
				}
				else if (hue>=60 && hue <=70)
				{
					hueColor=5;
				}
				else
				{
					hueColor=6;
				}
			LCD_Printf("Couleur = %d\n",hueColor);
			return hueColor;

}
