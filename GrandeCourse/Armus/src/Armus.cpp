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
	 boucleJeu();
	/*BTsendState(0);
	while (1)
	{
		LCD_ClearAndPrint("Valeur : %d",BTreadState());
		THREAD_MSleep(1000);
	}*/
	return 0;
}

