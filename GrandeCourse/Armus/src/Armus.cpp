/*
============================================================================
 Name : PID_Controller.cpp
 Author : Gabriel Bouchard
 Version : 0.5
 Description : Contient toutes les fonctions de d�placement
============================================================================
*/

// Include Files

#include <libarmus.h>

#define PI 3.141592
#define	TURNING_RADIUS 14.1 //TODO : Remesurer cette valeur

//Macro capteur de couleur
#define ADJD_S371_QR999_SADR 	0x74
#define CAP_RED					0x6
#define CAP_GREEN				0x7
#define CAP_BLUE				0x8
#define CAP_CLEAR				0x9
#define INTEGRATION_RED			10
#define INTEGRATION_GREEN		12
#define INTEGRATION_BLUE		14
#define INTEGRATION_CLEAR		16
#define ADJD_REG_CTRL			0
#define ADJD_REG_CONFIG			1
#define DATA_RED_LO				64
#define DATA_GREEN_LO			66
#define DATA_BLUE_LO			68
#define DATA_CLEAR_LO			70

#define CTRL_GSSR				0
#define CTRL_GOFS				1

#define CONFIG_TOFS				0
int adjd_dev;

//MACRO fait par nous
#define GAUCHE					-1
#define DROIT					1

#define ANALOGUE_BRUIT			6
#define ANALOGUE_5KZ			5

//Variables globales
float const cochesParCm = 2.6066666666666666666666666666666666666666;

// Variable pour les sons de d�part et fin
float tempDebut5Khz(0);
bool sonActif(0);

//TODO: Ajuster les constantes;
float  constanteProp = 1.0;
float  constanteInteg = 0.3;
float constanteDerive = 0;
int updateTime = 250;

struct infoRoue
{
	float puissanceMoteur;
	int cocheParcourueTotal;
	int cycles;
	float ValeurAttendue;
	int ValeurLue;
	int noMoteur;
	int valeurPrec;
	int cocheAParcourir;
} moteurGauche,moteurDroit;


// Global Functions

void boucleParcours(void);

void Avancer(float distance,float temps);
void Avancer(float vitesse);
void Tourner(float vitesse, float rayon, int direction);
void Tourner(float radian, float temps, float rayon);//direction 1=gauche 0=droite
void Reculer (float distance,float temps);

void AfficheRoue(infoRoue roue);
void PidController(infoRoue& roue);
void MesureRoue(infoRoue &roue);
void resetRoue(infoRoue &roue);

float detecte5khZ(void);

int math_ABS(int a);
void initialisationEncodeurs(void);
void mesureDeCoche(void);
//Obsolete
void TournerRayonNul(float radian, float vitesse, char direction);//direction 1=gauche 0=droite

//Fonctions Capteur de couleur
void adjd_SetRegister(unsigned char reg, unsigned char val);
void adjd_SetRegister16(unsigned char reg, int val);
unsigned char adjd_ReadRegister(unsigned char reg);
int adjd_ReadRegister16(unsigned char reg);
unsigned char cap_GetValue(unsigned char cap_address);
void cap_SetValue(unsigned char cap_address, unsigned char cap_value);
int integrationTime_GetValue(unsigned char address);
void integrationTime_SetValue(unsigned char address, int time_value);
void led_TurnOff();
void led_TurnOn();
void color_Read(int& data_red, int& data_blue, int& data_green, int& data_clear);
void color_ReadToCalibrate(int& data_red, int& data_blue, int& data_green, int& data_clear);
int color_Init(int& dev_handle);

int main()
{

	//TODO: thread pour jammer les autres avec le capteur infrarouge
	boucleParcours();

	return 0;
}

float detecte5khZ()
{
//mesures
	int bruit = ANALOG_Read(ANALOGUE_BRUIT);
	int son = ANALOG_Read(ANALOGUE_5KZ);

	if (bruit < son)
	{
		if (!sonActif) // part le chronom�tre
			tempDebut5Khz = SYSTEM_ReadTimerMSeconds();
		sonActif = true;
	}
	else
	{
		if (sonActif)
		{
			sonActif = false;
			return (SYSTEM_ReadTimerMSeconds() - tempDebut5Khz); // retourne le temps que le son � �t� �mis
		}
	}
	return 0;
}

void boucleParcours(void)
{

	/*----Initiation----*/
	ENCODER_Read(ENCODER_LEFT);
	ENCODER_Read(ENCODER_RIGHT);
	resetRoue(moteurDroit);
	resetRoue(moteurGauche);

	moteurGauche.noMoteur = MOTOR_LEFT;
	moteurDroit.noMoteur = MOTOR_RIGHT;

	/*----Attente du d�part----*/
	while (detecte5khZ() < 1000)
	{
		THREAD_MSleep(100);
	}

	/*----Parcours en tant que tel----*/
	do
	{
		//TODO faire boucle
		/*Faire boucle
			 *
			 * Mesure  des capteurs
			 *
			 * State -> selon la couleur
			 *
			 * Moteur real time
			 *
			 *
			 *optionnel : chronom�trer le temps que �a prends pour faire la boucle et la soustraire au THREAD_mSleep()
			 * */
		//Mesure


		//Switch case
		Tourner(15,0,GAUCHE);
		THREAD_MSleep(updateTime);
		//Mesures
		MesureRoue(moteurGauche);
		MesureRoue(moteurDroit);


		//Correction
		PidController(moteurGauche);
		PidController(moteurDroit);
		//TEST
		//AfficheRoue(moteurGauche);
		//AfficheRoue(moteurDroit);

	}while(detecte5khZ() < 3500);
	MOTOR_SetSpeed(MOTOR_LEFT,0);
	MOTOR_SetSpeed(MOTOR_RIGHT,0);

}



void Avancer(float vitesse)
{
	moteurGauche.cocheAParcourir = 0xfffff;
	moteurDroit.cocheAParcourir = 0xfffff;
	moteurGauche.ValeurAttendue = vitesse * updateTime/1000.0 ;
	moteurDroit.ValeurAttendue = vitesse * updateTime/1000.0;
}

void Avancer(float distance,float vitesse) // Vitesse : coches par secondes +/- 50
{
	//On met les compteurs � z�ro
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
		// Ce n'est plus lui qui va g�rer la condition de fin
	} while ((math_ABS(Gauche.cocheParcourueTotal) < math_ABS(nbCocheParcours)) || (math_ABS(Droit.cocheParcourueTotal) < math_ABS(nbCocheParcours)));

	MOTOR_SetSpeed(MOTOR_LEFT,0);
	MOTOR_SetSpeed(MOTOR_RIGHT,0);
}

//Ne marche pas pour l'instant
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

	moteurGauche.ValeurAttendue =vitesse * direction *((rayon + TURNING_RADIUS/2) * cochesParCm)/rayon * updateTime/1000.0;
	moteurDroit.ValeurAttendue = vitesse * direction *((rayon - TURNING_RADIUS/2) * cochesParCm)/rayon * updateTime/1000.0;
}
	else
	{
		moteurGauche.ValeurAttendue = vitesse * direction /2;
		moteurDroit.ValeurAttendue = -1*vitesse * direction /2;
	}


}
void Tourner(float radian, float vitesse, float rayon)
{

	//On met les compteurs � z�ro
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
	//On met les compteurs � z�ro
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



// Fonction de test

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
	//Partie P
	float P = (roue.ValeurAttendue - roue.ValeurLue)*constanteProp;


	//Partie I
	float I = (roue.cycles * roue.ValeurAttendue - roue.cocheParcourueTotal) * constanteInteg;


	//Partie D
	//TODO: tester la partie D

	float D = ((roue.ValeurAttendue - roue.ValeurLue) - (roue.ValeurAttendue - roue.valeurPrec)) * constanteDerive;

	//Fin
	roue.puissanceMoteur += P + I + D;

	// Certaine conditions
	if (math_ABS(roue.cocheAParcourir) < math_ABS(roue.cocheParcourueTotal)) //TODO � modifier pour fiter le real time
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
	roue.ValeurAttendue = 0;
	roue.ValeurLue = 0;
	roue.cocheAParcourir = 0xfffff;
	roue.cocheParcourueTotal = 0;
	roue.cycles = 0;
	roue.valeurPrec = 0;
}

//Fonctions Math�matiques

int math_ABS(int a)
{
	if (a < 0)
		return a * -1;
	return a;
}


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
