/*
============================================================================
 Name : testLigne.cpp
 Author :
 Version :
 Description : Hello world - Exe source file
============================================================================
*/

// Include Files

#include <libarmus.h>

// Global Functions

int call_suiveur();
void ajust_path();

int main()
{

 ajust_path();
	return 0 ;
}

int call_suiveur()   //fonction qui détecte ou est situer le noir
{

	int right;
	int center;
	int left;
	right = DIGITALIO_Read(9);
	center = DIGITALIO_Read(10) * 10;
	left = DIGITALIO_Read(11) * 100;

	int balance;
	balance = right + center + left;

	LCD_ClearAndPrint("Left = %d\nCenter = %d\nRight = %d\nBalance = %d\n",left/100 , center/10, right, balance);

	return balance;
}

void ajust_path()
{
	int suiveur;

	while(DIGITALIO_Read(BMP_REAR) == 0)
	{

		suiveur = call_suiveur();
		LCD_Printf("suiveur = %d\n",suiveur);

		switch(suiveur)
		{
			case 0:
				MOTOR_SetSpeed(MOTOR_RIGHT,-30);
				MOTOR_SetSpeed(MOTOR_LEFT,30);
				LCD_Printf("CAS 0");
				break;
			case 1 :
				MOTOR_SetSpeed(MOTOR_RIGHT,-20);
				MOTOR_SetSpeed(MOTOR_LEFT,30);
				LCD_Printf("CAS 1");
				break;
			case 11 :
				MOTOR_SetSpeed(MOTOR_RIGHT,50);
				MOTOR_SetSpeed(MOTOR_LEFT,30);
				LCD_Printf("CAS 11");
				break;
			case 100:
				MOTOR_SetSpeed(MOTOR_RIGHT,30);
				MOTOR_SetSpeed(MOTOR_LEFT,-20);
				LCD_Printf("CAS 100");
				break;
			case 110:
				MOTOR_SetSpeed(MOTOR_RIGHT,30);
				MOTOR_SetSpeed(MOTOR_LEFT,50);
				LCD_Printf("CAS 110");
				break;
			case 111 :
				MOTOR_SetSpeed(MOTOR_RIGHT,50);
				MOTOR_SetSpeed(MOTOR_LEFT,30);
				LCD_Printf("CAS 111");
				break;
			case 101 :
				MOTOR_SetSpeed(MOTOR_RIGHT,50);
				MOTOR_SetSpeed(MOTOR_LEFT,50);
				LCD_Printf("CAS 101");
				break;
			default :
				MOTOR_SetSpeed(MOTOR_RIGHT,0);
				MOTOR_SetSpeed(MOTOR_LEFT,0);
				LCD_Printf("CAS DEFAULT");
				break;

		}
	THREAD_MSleep(1000);

	}
	MOTOR_SetSpeed(MOTOR_RIGHT,0);
	MOTOR_SetSpeed(MOTOR_LEFT,0);

}




