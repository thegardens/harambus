/*
============================================================================
 Name : PID_Controller.cpp
 Author : Gabriel Bouchard
 Version : 0.5
 Description : Contient toutes les fonctions de déplacement
============================================================================
*/

// Include Files

#include <libarmus.h>
#include "grandecourse.h"
#include "Jeu.h"
#include "communication.h"

// Global Functions



//Fonctions Capteur de couleur


int main()
{
	THREAD_MSleep(500);
	bool running = true;
	do
	{
		 boucleJeu();
		 LCD_ClearAndPrint("Voulez-vous continuer jouer?\n\t1) oui\n\t2) non");

		 int choix;
		 do
		 {
		  choix = waitButtonInteraction();

		 }while (choix != 1 && choix != 2);
		 if (choix == 2)
		 {
			 running = false;
		 }
	}while (running);
	/*BTsendState(0);
	while (1)
	{
		LCD_ClearAndPrint("Valeur : %d",BTreadState());
		THREAD_MSleep(1000);
	}*/
	return 0;
}

