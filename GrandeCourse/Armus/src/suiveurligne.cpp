/*
 * suiveurligne.cpp
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */

#include "suiveurligne.h"
#include <libarmus.h>


int call_suiveur()   //fonction qui détecte ou est situer le noir
{

	int right;
	int center;
	int left;
	right = DIGITALIO_Read(9);
	center = DIGITALIO_Read(10) * 10;
	left = DIGITALIO_Read(11) * 100;

	int balance;
	balance = right + center + left;

	//LCD_ClearAndPrint("Left = %d\nCenter = %d\nRight = %d\nBalance = %d\n",left/100 , center/10, right, balance);

	return balance;
}

// Fonction de test

int ajust_path()
{
	int suiveur;



	suiveur = call_suiveur();
			LCD_Printf("suiveur = %d\n",suiveur);

			switch(suiveur)
			{
			case 0:
							MOTOR_SetSpeed(MOTOR_RIGHT,-35);
							MOTOR_SetSpeed(MOTOR_LEFT,30);
							//Tourner(10,0,DROIT);
							//LCD_Printf("Tourne à droite\n");
							//LCD_Printf("CAS 0");
							break;
						case 1 :
							MOTOR_SetSpeed(MOTOR_RIGHT,35);
							MOTOR_SetSpeed(MOTOR_LEFT,-30);
							//Tourner(10,0,GAUCHE);
							//LCD_Printf("Tourne à gauche\n");
							//LCD_Printf("CAS 1");
							break;
						case 11 :
							MOTOR_SetSpeed(MOTOR_RIGHT,35);
							MOTOR_SetSpeed(MOTOR_LEFT,-30);
							//Tourner(10,0,GAUCHE);
							//LCD_Printf("Tourne à gauche\n");
							//LCD_Printf("CAS 11");
							break;
						case 100:
							MOTOR_SetSpeed(MOTOR_RIGHT,-35);
							MOTOR_SetSpeed(MOTOR_LEFT,35);
							//Tourner(10,0,DROIT);
							//LCD_Printf("Tourne à droite\n");
							//LCD_Printf("CAS 100");
							break;
						case 110:
							MOTOR_SetSpeed(MOTOR_RIGHT,-35);
							MOTOR_SetSpeed(MOTOR_LEFT,35);
							//Tourner(10,0,DROIT);
							//LCD_Printf("Tourne à droite\n");
							//LCD_Printf("CAS 110");
							break;
						case 111 :
							MOTOR_SetSpeed(MOTOR_RIGHT,-35);
							MOTOR_SetSpeed(MOTOR_LEFT,-30);
							//Tourner(10,0,DROIT);
							//LCD_Printf("Tourne à droite\n");
							//LCD_Printf("CAS 111");
							break;
						case 101 :
							MOTOR_SetSpeed(MOTOR_RIGHT,-35);
							MOTOR_SetSpeed(MOTOR_LEFT,-35);
							//Avancer(15);
							//LCD_Printf("avance\n");
							//LCD_Printf("CAS 101");
							break;
						default :
							MOTOR_SetSpeed(MOTOR_RIGHT,0);
							MOTOR_SetSpeed(MOTOR_LEFT,0);
							//Avancer(0);
							//LCD_Printf("STOP\n");
							//LCD_Printf("CAS DEFAULT");
							break;
			}
			return suiveur;
		}
