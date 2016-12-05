/*
 * Jeu.h
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */

#ifndef JEU_H_
#define JEU_H_
//STRUCT
struct questionnaire{
	char enonce[255];
	char reponse1[255];
	char reponse2[255];
	char reponse3[255];
	char reponse4[255];
	char bonne_reponse[255];
};
//MACRO STATE
#define STATE_DEBUT		0
#define STATE_QUESTION	1
#define STATE_DE		2
#define STATE_AVANCE	3
#define STATE_ATTENTE	4
#define STATE_GAGNANT	5
#define STATE_PERDANT	6

//MACRO tout court


//SIGNATURE FONCTIONS
void boucleJeu();
int random_dice();
int gestion_questions(char fichier_question[],struct questionnaire liste[]);
int waitButtonInteraction();
void avanceCase(int nbCase);

#endif /* JEU_H_ */
