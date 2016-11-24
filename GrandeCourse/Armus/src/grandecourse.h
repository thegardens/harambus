/*
 * grandecourse.h
 *
 *  Created on: 2016-11-23
 *      Author: boug2629
 */

#ifndef GRANDECOURSE_H_
#define GRANDECOURSE_H_

// Variable pour les sons de départ et fin


#define ANALOGUE_BRUIT			6
#define ANALOGUE_5KZ			5

#define STATE_START				0
#define STATE_FIRST_TURN		1
#define STATE_TAPIS				2
#define STATE_MURET				3
#define STATE_LIGNE				4
#define STATE_RAMP				5
#define STATE_FINISH			6
#define STATE_END				7
#define STATE_DEBUT_DROIT		8

float detecte5khZ();
float detecte5khZ(int ms);
void boucleParcours(void);


#endif /* GRANDECOURSE_H_ */
