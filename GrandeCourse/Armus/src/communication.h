/*
 * communication.h
 *
 *  Created on: 2016-12-03
 *      Author: boug2629
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_



void BTsendState(int state, bool is2Joueurs = true);

int BTreadState( bool is2Joueurs = true);


#endif /* COMMUNICATION_H_ */
