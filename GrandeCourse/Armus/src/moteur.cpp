/*
 * moteur.cpp
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */

#include "moteur.h"
#include <libarmus.h>
#include "constante.h"


//TODO: Ajuster les constantes;
float  constanteProp = 1.8;
float  constanteInteg = 0.05;
float constanteDerive = 0;

int isReversed = 1;

int updateTime = 100;

float const cochesParCm = 2.6066666666666666666666666666666666666666;

struct infoRoue  moteurGauche,moteurDroit;
void Avancer(float vitesse)
{
	resetRoue(moteurGauche);
	resetRoue(moteurDroit);
	moteurGauche.cocheAParcourir = 0xfffff;
	moteurDroit.cocheAParcourir = 0xfffff;
	moteurGauche.ValeurAttendue = isReversed * vitesse * updateTime/1000.0 ;
	moteurDroit.ValeurAttendue = isReversed * vitesse * updateTime/1000.0;
}

void Avancer(float distance,float vitesse) // Vitesse : coches par secondes +/- 50
{
	//On met les compteurs à zéro
	ENCODER_Read(ENCODER_LEFT);
	ENCODER_Read(ENCODER_RIGHT);

	//Initialisation des variables
	infoRoue Gauche = {0};
	infoRoue Droit = {0};


	Gauche.noMoteur = MOTOR_LEFT;
	Droit.noMoteur = MOTOR_RIGHT;

	//Conversion distance en coche
	int nbCocheParcours = distance * cochesParCm;

	Gauche.cocheAParcourir = nbCocheParcours;
	Droit.cocheAParcourir = nbCocheParcours;

	Gauche.ValeurAttendue = vitesse * updateTime/1000.0 ;
	Droit.ValeurAttendue = vitesse * updateTime/1000.0;
	//TODO: Mettre la boucle en fonction
	do
	{
		//Attente
		THREAD_MSleep(updateTime);



		//Mesures
		MesureRoue(Gauche);
		MesureRoue(Droit);


		//Correction
		PidController(Gauche);
		PidController(Droit);
		//TEST
		AfficheRoue(Gauche);
		AfficheRoue(Droit);
		// Ce n'est plus lui qui va gérer la condition de fin
	} while ((math_ABS(Gauche.cocheParcourueTotal) < math_ABS(nbCocheParcours)) || (math_ABS(Droit.cocheParcourueTotal) < math_ABS(nbCocheParcours)));

	MOTOR_SetSpeed(MOTOR_LEFT,0);
	MOTOR_SetSpeed(MOTOR_RIGHT,0);
}

void Reculer (float vitesse)
{
	Avancer(-vitesse);
}
void Reculer (float distance,float vitesse)
{
	Avancer(-distance, -vitesse);
}


void Tourner(float vitesse, float rayon, int direction)
{

	moteurGauche.cocheAParcourir = 0xfffff;
	moteurDroit.cocheAParcourir = 0xfffff;
if (rayon != 0)
{

	moteurGauche.ValeurAttendue = isReversed * vitesse * direction *((rayon + TURNING_RADIUS/2) * cochesParCm)/rayon * updateTime/1000.0 /4;
	moteurDroit.ValeurAttendue = isReversed * vitesse * direction *((rayon - TURNING_RADIUS/2) * cochesParCm)/rayon * updateTime/1000.0 /4;
}
	else
	{
		moteurGauche.ValeurAttendue = isReversed * vitesse * direction /4;
		moteurDroit.ValeurAttendue = isReversed *-1*vitesse * direction /4;
	}


}
void Tourner(float radian, float vitesse, float rayon)
{

	//On met les compteurs à zéro
		ENCODER_Read(ENCODER_LEFT);
		ENCODER_Read(ENCODER_RIGHT);


	//Initialisation des variables
		infoRoue Gauche = {0};
		infoRoue Droit = {0};

		if (radian < 0)
		{
			radian *= -1;
			Gauche.cocheAParcourir = radian * (rayon + TURNING_RADIUS/2) * cochesParCm;
			Droit.cocheAParcourir = radian * (rayon - TURNING_RADIUS/2) * cochesParCm;
		}
		else
		{
			Gauche.cocheAParcourir = radian * (rayon - TURNING_RADIUS/2) * cochesParCm;
			Droit.cocheAParcourir = radian * (rayon + TURNING_RADIUS/2) * cochesParCm;
		}

		Gauche.ValeurAttendue = Gauche.cocheAParcourir/(rayon*radian/vitesse) * updateTime/1000.0;
		Droit.ValeurAttendue =  Droit.cocheAParcourir/(rayon*radian/vitesse) * updateTime/1000.0;

		Gauche.noMoteur = MOTOR_LEFT;
		Droit.noMoteur = MOTOR_RIGHT;

		do
		{
			THREAD_MSleep(updateTime);
			//Mesures
			MesureRoue(Gauche);
			MesureRoue(Droit);

			PidController(Gauche);
			PidController(Droit);

			//TEST
			AfficheRoue(Gauche);
			AfficheRoue(Droit);
		} while ( math_ABS(Droit.cocheParcourueTotal) < math_ABS(Droit.cocheAParcourir) || math_ABS(Gauche.cocheParcourueTotal) < math_ABS(Gauche.cocheAParcourir) );


			MOTOR_SetSpeed(MOTOR_LEFT,0);
			MOTOR_SetSpeed(MOTOR_RIGHT,0);

}


//OBSOLETE
void TournerRayonNul(float radian, float vitesse, char direction)//direction 0=gauche 1=droite
{
	MOTOR_SetSpeed(MOTOR_LEFT,0);
	MOTOR_SetSpeed(MOTOR_RIGHT,0);
	//On met les compteurs à zéro
		ENCODER_Read(ENCODER_LEFT);
		ENCODER_Read(ENCODER_RIGHT);

	//Initialisation des variables
		infoRoue Gauche = {0};
		infoRoue Droit = {0};
		float arc = radian*TURNING_RADIUS / 2;
		int nbCocheArc = arc * cochesParCm;

		if(direction==1)
		{
			Droit.noMoteur = MOTOR_RIGHT;
			MOTOR_SetSpeed(MOTOR_RIGHT,vitesse);
			Gauche.noMoteur = MOTOR_LEFT;
			MOTOR_SetSpeed(MOTOR_LEFT,-vitesse);
			do
			{
			THREAD_MSleep(10);
			//Mesures
			Droit.ValeurLue = ENCODER_Read(ENCODER_RIGHT);
			Droit.cocheParcourueTotal += Droit.ValeurLue;
			Gauche.ValeurLue = ENCODER_Read(ENCODER_LEFT);
			Gauche.cocheParcourueTotal += Gauche.ValeurLue;
			} while ( Droit.cocheParcourueTotal < nbCocheArc || Gauche.cocheParcourueTotal < nbCocheArc);
			MOTOR_SetSpeed(MOTOR_RIGHT,0);
			MOTOR_SetSpeed(MOTOR_LEFT,0);
		}
		else if(direction == 0)
		{
			Droit.noMoteur = MOTOR_RIGHT;
			MOTOR_SetSpeed(MOTOR_RIGHT,-vitesse);
			Gauche.noMoteur = MOTOR_LEFT;
			MOTOR_SetSpeed(MOTOR_LEFT,vitesse);
			do
			{
				THREAD_MSleep(10);
				//Mesures
				Droit.ValeurLue = ENCODER_Read(ENCODER_RIGHT);
				Droit.cocheParcourueTotal += Droit.ValeurLue;
				Gauche.ValeurLue = ENCODER_Read(ENCODER_LEFT);
				Gauche.cocheParcourueTotal += Gauche.ValeurLue;
			} while ( Droit.cocheParcourueTotal < nbCocheArc || Gauche.cocheParcourueTotal < nbCocheArc);
			MOTOR_SetSpeed(MOTOR_RIGHT,0);
			MOTOR_SetSpeed(MOTOR_LEFT,0);

		}

}






void mesureDeCoche(void)
{
	ENCODER_Read(ENCODER_LEFT);
	ENCODER_Read(ENCODER_RIGHT);
	int encG(0),encD(0);
	while(1)
	{
		THREAD_MSleep(1000);
		int G = ENCODER_Read(ENCODER_LEFT);
		int D = ENCODER_Read(ENCODER_RIGHT);
		float vitesse = (G+D)/(2);
		encG += G;
		encD += D;
		LCD_ClearAndPrint("---------Totaux--------\n   %d        %d\nMoyenne : %8.1f Coches\n\nVitesse : %6.3f Coches/sec",encG,encD,(float)(encG+encD)/2,vitesse);
	}

}




/* TRUC POUR LES ROUE */


void PidController(infoRoue& roue)
{
	if(roue.ValeurAttendue != 0) // Empèche le bug qu'il commence à avancer lorque l'on pousse dessus
	{

		//Partie P
		float P = (roue.ValeurAttendue - roue.ValeurLue)*constanteProp;


		//Partie I
		float I = (roue.cycles * roue.ValeurAttendue - roue.cocheParcourueTotal) * constanteInteg;


		//Partie D
		//TODO: tester la partie D

		float D = ((roue.ValeurAttendue - roue.ValeurLue) - (roue.ValeurAttendue - roue.valeurPrec)) * constanteDerive;

		//Fin
		float correction = P + I + D;

		if (correction > 100)
		{
			correction = 100;
		}
		else if (correction < -100)
		{
			correction = -100;
		}

		roue.puissanceMoteur += correction;

		// Certaine conditions
		if (math_ABS(roue.cocheAParcourir) < math_ABS(roue.cocheParcourueTotal)) //TODO à modifier pour fiter le real time
			roue.puissanceMoteur = 0;
		else if(roue.puissanceMoteur > 100)
		{
			roue.puissanceMoteur = 100;
			LCD_Printf("ERROR :\tMotor overload\n");
		}
		else if (roue.puissanceMoteur < -100)
		{
			roue.puissanceMoteur = -100;
			LCD_Printf("ERROR :\tMotor overload\n");
		}
		MOTOR_SetSpeed(roue.noMoteur,roue.puissanceMoteur);
	}
}

void MesureRoue(infoRoue &roue) //TODO Adapter pour resetter les cycles entre chaque
{
	roue.cycles++;
	roue.valeurPrec = roue.ValeurLue;
	roue.ValeurLue = ENCODER_Read(roue.noMoteur - 6);

	if(roue.ValeurAttendue < 0)
		roue.ValeurLue *= -1;
	roue.cocheParcourueTotal += roue.ValeurLue;
}

void AfficheRoue(infoRoue roue)
{
	LCD_Printf("noMoteur : %d -> %.3f\nValeur Lue : %d\nValeur Attendue : %f\n",roue.noMoteur,roue.puissanceMoteur,roue.ValeurLue,roue.ValeurAttendue);
}

void resetRoue(infoRoue& roue) // TODO: savoir si l'on a de besoin de resetter puissance moteur
{
	//roue.puissanceMoteur = 0;
	//MOTOR_SetSpeed(roue.noMoteur,0);
	//ENCODER_Read(roue.noMoteur - 6);
	roue.ValeurAttendue = 0;
	roue.ValeurLue = 0;
	roue.cocheAParcourir = 0xfffff;
	roue.cocheParcourueTotal = 0;
	roue.cycles = 0;
	roue.valeurPrec = 0;
}

//Fonctions Mathématiques

int math_ABS(int a)
{
	if (a < 0)
		return a * -1;
	return a;
}






