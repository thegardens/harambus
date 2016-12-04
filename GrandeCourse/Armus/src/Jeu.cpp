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

int const nbMaxQuestions = 50;
struct questionnaire tab_questions[nbMaxQuestions]; //TODO: Mettre dynamique


void boucleJeu()
{
	int state = STATE_DEBUT;
	int resultatDe;
	int nbTours = 1;

	gestion_questions("media/usb0/ARMUS/question.txt", tab_questions);

	while(1)
	{

		switch (state)
		{
		case STATE_DEBUT:
			LCD_ClearAndPrint("Bienvenue au jeu de harambus\n");
			LCD_Printf("Appuyer sur un bouton...");
			waitButtonInteraction();
			state = STATE_QUESTION;
			break;


		case STATE_QUESTION:
			LCD_ClearAndPrint("Questions #%d\n",nbTours);
			int choix;

			LCD_Printf("%s", tab_questions[nbTours-1].enonce);
			LCD_Printf("1 : %s", tab_questions[nbTours-1].reponse1);
			LCD_Printf("2 : %s", tab_questions[nbTours-1].reponse2);
			LCD_Printf("3 : %s", tab_questions[nbTours-1].reponse3);
			LCD_Printf("4 : %s", tab_questions[nbTours-1].reponse4);
			THREAD_MSleep(1000);
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
				LCD_Printf("MAUVAISE REPONSE loser\n");
			}
			else
				LCD_Printf("BONNE REPONSE\n");

			THREAD_MSleep(2000);
			break;


		case STATE_DE:
			resultatDe = random_dice();

			THREAD_MSleep(1000);
			state = STATE_AVANCE;
			break;
		case STATE_AVANCE:
			avanceCase(resultatDe);
			state = STATE_ATTENTE;
			break;

		case STATE_ATTENTE:
			nbTours++;
			LCD_ClearAndPrint("Appuyer sur un bouton...");
			waitButtonInteraction();
			state = STATE_QUESTION;
			break;

		case STATE_FIN:
			LCD_ClearAndPrint("ON A GAGNE!!!");
			break;
		}
	}

}


void avanceCase(int nbCase)
{
	int casePasser = 0;
	int i = 0;
	int couleur = testCouleur();
	while (nbCase > casePasser)
	{
		ajust_path();
		if(i % 6 == 0)
		{
			int echantillonage = testCouleur();
			if(couleur != echantillonage)
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
	ans=rdm % 6;



	if (ans==1)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |___________|\n");

	}
	if (ans==2)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0         |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        |         0 |\n");
		LCD_Printf("        |___________|\n");

	}
	if (ans==3)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0         |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        |         0 |\n");
		LCD_Printf("        |___________|\n");
	}
	if (ans==4)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");
	}

	if (ans==5)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |     0     |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");
	}
	if (ans==0 || ans==6)
	{


		LCD_Printf("         ___________\n");
		LCD_Printf("        |           |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        | 0       0 |\n");
		LCD_Printf("        |___________|\n");
		ans=6;
	}


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
	while(1)
	{
		if(DIGITALIO_Read(BOUTON_1))
			return 1;
		if(DIGITALIO_Read(BOUTON_2))
			return 2;
		if(DIGITALIO_Read(BOUTON_3))
			return 3;
		if(DIGITALIO_Read(BOUTON_4))
			return 4;
		THREAD_MSleep(100);
	}
}
