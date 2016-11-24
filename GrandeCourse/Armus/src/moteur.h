/*
 * moteur.h
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */

#ifndef MOTEUR_H_
#define MOTEUR_H_




//MACRO fait par nous
#define GAUCHE					-1
#define DROIT					1



//Variables globales








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
};


void Avancer(float distance,float temps);
void Avancer(float vitesse);
void Tourner(float vitesse, float rayon, int direction);
void Tourner(float radian, float temps, float rayon);//direction 1=gauche 0=droite
void Reculer (float vitesse);
void Reculer (float distance,float vitesse);

void AfficheRoue(infoRoue roue);
void PidController(infoRoue& roue);
void MesureRoue(infoRoue &roue);
void resetRoue(infoRoue &roue);


int math_ABS(int a);
void initialisationEncodeurs(void);



void mesureDeCoche(void);
//Obsolete
void TournerRayonNul(float radian, float vitesse, char direction);//direction 1=gauche 0=droite


#endif /* MOTEUR_H_ */
