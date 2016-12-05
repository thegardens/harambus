/*
============================================================================
 Name : Armus.cpp
 Author :
 Version :
 Description : Hello world - Exe source file
============================================================================
*/

// Include Files

#include <libarmus.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "son.h"
// Global Functions



int testSon()
{
/*
	THREAD_MSleep(2000);
	LCD_ClearAndPrint("Debut Play Song\n");
	Play_Song("Debut.wav");
	Play_Song("Bon.wav");
	Play_Song("Mal.wav", 1000);
	Play_Song("Tour.wav", 1000);
	Play_Song("Perd.wav", 2000);
	Play_Song("Win.wav", 10000);
	Play_Song("Theme.wav", 84000);
	LCD_Printf("Fin");
*/
	/*

	//AUDIO_PlayFile("DK.wav");

	AUDIO_SetVolume(100);
	AUDIO_PlayFile("New_DK.wav");
	THREAD_MSleep(1500);

	*/


	return 0;
}

void Play_Song(char Song_Name[])
{
	AUDIO_SetVolume(100);
	AUDIO_PlayFile(Song_Name);


}
