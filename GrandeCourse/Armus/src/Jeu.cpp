/*
 * Jeu.cpp
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */
#include "Jeu.h"
#include <libarmus.h>
#include "suiveurligne.h"
#include "constante.h"
#include "capteurcouleur.h"
#include "son.h"
#include "communication.h"

int const nbMaxQuestions = 50;
int const nbCaseAParcourir = 10;
struct questionnaire tab_questions[nbMaxQuestions]; //TODO: Mettre dynamique


void boucleJeu()
{

	LCD_SetMonitorMode(MONITOR_OFF);
	int state = STATE_DEBUT;
	int resultatDe;
	int nbTours = 1;
	int caseParcourue = 0;
	bool running = true;
	gestion_questions("media/usb0/ARMUS/question.txt", tab_questions);
	BTsendState(state);
	while(running )
	{

		switch (state)
		{



		case STATE_DEBUT:
			SYSTEM_ResetTimer(); // empèche un overflow du timer
			//LCD_ClearAndPrint("");
			//LCD_ClearAndPrint("Bienvenue au jeu de harambus\n");
			//LCD_Printf("Appuyer sur un bouton pour COMMENCER");

			LCD_PrintBmp("Debut.bmp");

			waitButtonInteraction();
			if(BTreadState() == STATE_DEBUT)
			{
				state = STATE_ATTENTE;
				nbTours--;
			}
			else if(BTreadState() == STATE_ATTENTE)
			{
				Play_Song("Debut.wav");
				state = STATE_QUESTION;

			}
			BTsendState(state);
			break;




		case STATE_QUESTION:
			LCD_ClearAndPrint("Questions #%d\n",nbTours);
			int choix;

			LCD_Printf("%s", tab_questions[nbTours-1].enonce);
			LCD_Printf("1 : %s", tab_questions[nbTours-1].reponse1);
			LCD_Printf("2 : %s", tab_questions[nbTours-1].reponse2);
			LCD_Printf("3 : %s", tab_questions[nbTours-1].reponse3);
			LCD_Printf("4 : %s", tab_questions[nbTours-1].reponse4);
			choix = waitButtonInteraction();

			switch (choix){
			case 1:
				if(strcmp(tab_questions[nbTours-1].reponse1, tab_questions[nbTours-1].bonne_reponse)==0)
					state = STATE_DE;
				break;
			case 2:
				if(strcmp(tab_questions[nbTours-1].reponse2 , tab_questions[nbTours-1].bonne_reponse)==0)
					state = STATE_DE;
				break;
			case 3:
				if(strcmp(tab_questions[nbTours-1].reponse3 , tab_questions[nbTours-1].bonne_reponse)==0)
					state = STATE_DE;
				break;
			case 4:
				if(strcmp(tab_questions[nbTours-1].reponse4, tab_questions[nbTours-1].bonne_reponse)==0)
					state = STATE_DE;
				break;
			}
			if(state != STATE_DE)
			{
				state = STATE_ATTENTE;
				//LCD_Printf("MAUVAISE REPONSE loser\n");
				LCD_PrintBmp("Mal.bmp");
				Play_Song("Mal.wav");
			}
			else
			{
				LCD_Printf("BONNE REPONSE\n");
				LCD_PrintBmp("Bon.bmp");
				Play_Song("Bon.wav");
			}

			THREAD_MSleep(1000);

			BTsendState(state);
			break;


		case STATE_DE:
			resultatDe = random_dice();

			THREAD_MSleep(1000);
			state = STATE_AVANCE;

			BTsendState(state);
			break;




		case STATE_AVANCE:
			//Vérif si gagant
			if(nbCaseAParcourir <= caseParcourue + resultatDe )
			{
				resultatDe = nbCaseAParcourir - caseParcourue;
				state = STATE_GAGNANT;
			}

			avanceCase(resultatDe);
			caseParcourue += resultatDe;
			if(state != STATE_GAGNANT)
				state = STATE_ATTENTE;

			BTsendState(state);
			break;




		case STATE_ATTENTE:

			SYSTEM_ResetTimer(); // empèche un overflow du timer

			LCD_PrintBmp("Tour.bmp");
			//LCD_ClearAndPrint("Appuyer sur un bouton...");
			//waitButtonInteraction();
			while (BTreadState() != STATE_QUESTION)
			{
				BTsendState(state);
				THREAD_MSleep(300);
				//LCD_ClearAndPrint("State: %d\nState autre joueur : %d", state, BTreadState());
			}
			while (state != STATE_QUESTION && state != STATE_PERDANT){
				if(BTreadState()==STATE_ATTENTE)
				{
					nbTours += 2;
					state = STATE_QUESTION;
					Play_Song("Tour.wav");

				}
				if(BTreadState()==STATE_GAGNANT)
				{
					state = STATE_PERDANT;
				}

				BTsendState(state);
				THREAD_MSleep(300);
			}
			break;




		case STATE_GAGNANT:
			LCD_PrintBmp("Victoire.bmp");
			Play_Song("Win.wav");
			//LCD_ClearAndPrint("ON A GAGNE!!!");

			BTsendState(state);
			running = false; // fin de jeu retour au main
			break;


		case STATE_PERDANT:
					LCD_PrintBmp("Perdu.bmp");
					Play_Song("Perd.wav");
					//LCD_ClearAndPrint("ON A Perdu!!!");

					BTsendState(state);
					running = false; // fin de jeu retour au main
					break;
		}

		//LCD_ClearAndPrint("State: %d\nState autre joueur : %d", state, BTreadState());
		THREAD_MSleep(750);
	}
	THREAD_MSleep(3000);

}


void avanceCase(int nbCase)
{
	int casePasser = 0;
	int i = 0;
	int couleur;
	do
	{
		couleur = testCouleur();

	}while (couleur != 2 && couleur != 4);

	while (nbCase > casePasser)
	{
		ajust_path();
		if(i % 6 == 0)
		{
			int echantillonage = testCouleur();
			if(couleur != echantillonage && (echantillonage == 2 || echantillonage == 4))
			{
				casePasser++;
				LCD_ClearAndPrint("Case passee : %d",casePasser);
				couleur = echantillonage;
			}
		}
		else
		{
			THREAD_MSleep(100);
		}

		i++;
	}
	MOTOR_SetSpeed(MOTOR_LEFT,0);
	MOTOR_SetSpeed(MOTOR_RIGHT,0);
}

int random_dice()
{

	int rdm=0;
	int ans=0;

	LCD_ClearAndPrint("");


	rdm=SYSTEM_ReadTimerMSeconds();
	ans=rdm % 4 + 1;



	if (ans==1)
	{

		/*
		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |___________|\n");
		*/
		LCD_PrintBmp("Dice1.bmp");

	}
	if (ans==2)
	{

		/*
		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0         |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |         0 |\n");
		LCD_Printf("        |___________|\n");	*/
		LCD_PrintBmp("Dice2.bmp");

	}
	if (ans==3)
	{

		/*
		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0         |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        |         0 |\n");
		LCD_Printf("        |___________|\n");*/
		LCD_PrintBmp("Dice3.bmp");
	}
	if (ans==4)
	{

		/*
		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");*/
		LCD_PrintBmp("Dice4.bmp");
	}
/*
	if (ans==5)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");
		LCD_PrintBmp("Dice5.bmp");
	}
	if (ans==0 || ans==6)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");
		LCD_PrintBmp("Dice6.bmp");
		ans=6;
	}
*/

	//LCD_Printf("%d %d",rdm,ans); //Affiche le temps et la valeur du dé

	return ans;

}


int gestion_questions(char fichier_question[],struct questionnaire liste[nbMaxQuestions]){

	FILE* fichier_i;
	fichier_i= fopen(fichier_question,"r");
	int i = 0;
	char v_string[nbMaxQuestions];

	if(fichier_i==NULL){

	return 0;
	}

	while(!feof(fichier_i))
	{
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].enonce,v_string);
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].reponse1,v_string);
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].reponse2,v_string);
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].reponse3,v_string);
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].reponse4,v_string);
		fgets(v_string,sizeof(v_string),fichier_i);
		strcpy(liste[i].bonne_reponse,v_string);

		i++;
	}

	fclose(fichier_i);
	return 1;

}

int waitButtonInteraction()
{
	int threshold = 700;
	while(1)
	{
		if(ANALOG_Read(BOUTON_1) > threshold)
			return 1;
		if(ANALOG_Read(BOUTON_2) > threshold)
			return 2;
		if(ANALOG_Read(BOUTON_3) > threshold)
			return 3;
		if(ANALOG_Read(BOUTON_4) > threshold)
			return 4;
		THREAD_MSleep(100);
	}
}
