#include "grandecourse.h"
#include <libarmus.h>
#include "moteur.h"
#include "constante.h"
#include "capteurcouleur.h"
#include "suiveurligne.h"


float tempDebut5Khz(0);
bool sonActif(0);

float detecte5khZ()
{
//mesures
	int bruit = ANALOG_Read(ANALOGUE_BRUIT);
	int son = ANALOG_Read(ANALOGUE_5KZ);

	if (bruit < son)
	{
		if (!sonActif) // part le chronomètre
			tempDebut5Khz = SYSTEM_ReadTimerMSeconds();
			sonActif = true;
	}
	else
	{
		if (sonActif)
		{
			sonActif = false;
			return (SYSTEM_ReadTimerMSeconds() - tempDebut5Khz); // retourne le temps que le son à été émis
		}
	}
	return 0;
}

float detecte5khZ(int ms)
{
//mesures
	int bruit = ANALOG_Read(ANALOGUE_BRUIT);
	int son = ANALOG_Read(ANALOGUE_5KZ);
	int temps_signal;
	temps_signal = SYSTEM_ReadTimerMSeconds() - tempDebut5Khz;

	if (bruit < son)
	{
		if (!sonActif){ // part le chronomètre
			tempDebut5Khz = SYSTEM_ReadTimerMSeconds();
			sonActif = true;
		}
		else if(temps_signal >= ms){
			sonActif = false;
			return (ms)	; // retourne le temps que le son à été émis
		}
	}
	else
	{
		if (sonActif)
		{
			sonActif = false;
			return (SYSTEM_ReadTimerMSeconds() - tempDebut5Khz); // retourne le temps que le son à été émis
		}
	}
	return 0;
}

void boucleParcours(void)
{
	//Variables de parcours
	int state = STATE_START;
	int code;
	int couleur;
	while (state != STATE_END)
	{


		switch (state)
		{


		case STATE_START:
			LCD_ClearAndPrint("Wait...");
			if(detecte5khZ(250) >= 250)
			{
				state = STATE_FIRST_TURN;
			}
			break;


		case STATE_DEBUT_DROIT:
				LCD_ClearAndPrint("Debut Droit");
			osc_couleur(testCouleur());
				code = call_suiveur();
				if (code/100 == 0)
				MOTOR_SetSpeed(MOTOR_RIGHT,0);
				if(code%10 == 0)
					MOTOR_SetSpeed(MOTOR_LEFT,0);
				if(code == 0)
				{
					Avancer(3,15);
					state = STATE_FIRST_TURN;
				}

				break;



		case STATE_FIRST_TURN:
			LCD_ClearAndPrint("First turn");
			osc_couleur(testCouleur());
			code = call_suiveur();
			if (!code /100)
				MOTOR_SetSpeed(MOTOR_RIGHT,0);
			if(!code % 10)
				MOTOR_SetSpeed(MOTOR_LEFT,0);
			if(code == 0)
			{
				Reculer(3,15);
				code = call_suiveur();
				if (code==0)
					state = STATE_TAPIS;
			}
			break;



		case STATE_TAPIS:
			LCD_ClearAndPrint("Tapis");
			MOTOR_SetSpeed(MOTOR_RIGHT,-50);
			MOTOR_SetSpeed(MOTOR_LEFT,-45);
			code = call_suiveur();
			if (code /100)
				MOTOR_SetSpeed(MOTOR_RIGHT,0);
			if(code % 10)
				MOTOR_SetSpeed(MOTOR_LEFT,0);
			if(code == 111)
			{
				state = STATE_MURET;
			}
			break;


		case STATE_MURET:

			LCD_ClearAndPrint("Muret");
			couleur = testCouleur();
			osc_couleur(couleur);
			code = call_suiveur();
			if(code < 111 || couleur == 5)
				state = STATE_LIGNE;
			break;
		case STATE_LIGNE:
			LCD_ClearAndPrint("Ligne");
			//Ligne de poudre
			if(couleur==1)
			osc_couleur(couleur);

			code = ajust_path();
			couleur = testCouleur();
			//mettre correction pour le rouge p-t
			if (couleur == 3 || couleur == 4||couleur ==2) // a trouver
				state = STATE_RAMP;
			break;
		case STATE_RAMP:
			LCD_ClearAndPrint("Rampe");
			Tourner(PI/4,10,0);
			MOTOR_SetSpeed(MOTOR_LEFT,-50);
			MOTOR_SetSpeed(MOTOR_RIGHT,-50);
			break;
		case STATE_FINISH:
			LCD_ClearAndPrint("Fini");
			MOTOR_SetSpeed(MOTOR_LEFT,0);
			MOTOR_SetSpeed(MOTOR_RIGHT,0);
			state = STATE_END;
			break;

		}

		if(state != STATE_START && detecte5khZ(3000) >= 3000)
				state = STATE_FINISH;


		//THREAD_MSleep(updateTime);
	}


}
