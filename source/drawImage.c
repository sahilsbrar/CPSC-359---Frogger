#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>	//sounds
#include "framebuffer.h"
#include "drawImage.h"

// levels
#include "lvl1.h"			// lvl_one.pixel_data
#include "lvl2.h"			// lvl_two.pixel_data
#include "lvl3.h"			// lvl_three.pixel_data
#include "lvl4.h"			// lvl_four.pixel_data

// frogs
#include "frogFWD.h"		// frogFWD.pixel_data
#include "frogBWD.h"		// frogBWD.pixel_data
#include "frogLWD.h"		// frogLWD.pixel_data
#include "frogRWD.h"		// frogRWD.pixel_data
#include "frogDead.h"		// frogDead.pixel_data

// frame borders
#include "leftBorder.h"		// leftBorder.pixel_data
#include "rightBorder.h"	// rightBorder.pixel_data

// obstacles
#include "lvl1Obs.h"		// lvlOne_Obs.pixel_data
#include "lvl2Obs.h"		// lvlTwo_Obs.pixel_data
#include "lvl3Obs.h"		// lvlThree_Obs.pixel_data
#include "lvl4Obs.h"		// lvlFour_Obs.pixel_data

// numbers and timer bar
#include "numbersImg.h"		// numbersImg.pixel_data
#include "timerBar.h"		// timerBar.pixel_data

// pause
#include "pauseResume.h"	// pauseResume.pixel_data
#include "pauseQuit.h"		// pauseQuit.pixel_data

// win/lose
#include "winScreen.h"		// winScreen.pixel_data
#include "loseScreen.h"		// loseScreen.pixel_data

// main menu
#include "startImg.h"		// startImg.pixel_data
#include "quitImg.h"		// quitImg.pixel_data

/* Definitions */

//string hop = Path.Combine(Environment.CurrentDirectory, "hop.wav");


//int playSound( char *filename ) {
	//char command[256];
	//int status;
	
	/* create command to execute */
	//sprintf( command, "aplay -c 1 -q -t wav %s", filename );
	
	/* play sound */
	//status = system( command );
	
	//return status;	
//}



struct fbs framebufferstruct;

int level = 1;
int movesLeft = 50;
int movesTaken = 0;
int lastPressedX = 1240;	// was 1200 (offset by +39)
int lastPressedY = 537;		// was 538 (offset by -3)
bool startGame = false;
bool quitGame = false;

bool getStart(){
	return startGame;
}

bool getQuit(){
	return quitGame;
}

int drawMainMenu(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;

	if(buttonPressed == 5){
		startGame = true;
		quitGame = false;

		short int *background=(short int *) startImg.pixel_data;
		
		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
	}else{
		startGame = false;
		quitGame = true;
		short int *background=(short int *) quitImg.pixel_data;

		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

int drawGameScreen(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	if(lastPressedY <= 155 && buttonPressed == 5){
		level = level + 1;
		lastPressedY = 601;	// was 602
	}

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;

	if(level == 1){
		short int *background=(short int *) lvl_one.pixel_data;

		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}

	} else if(level == 2){
		short int *background=(short int *) lvl_two.pixel_data;

		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}

	} else if(level == 3){
		short int *background=(short int *) lvl_three.pixel_data;

		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
	}else{
		short int *background=(short int *) lvl_four.pixel_data;

		for (int y = 0; y < 720; y++)//30 is the image height
		{
			for (int x = 600; x < 1880; x++) // 30 is image width
			{	
					pixel->color = background[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);

	return 0;
}

int drawFrames(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixelLeft;
	pixelLeft = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;
	/* left frame */
	short int *leftFrame=(short int *) leftBorder.pixel_data;
	for (int y = 0; y < 720; y++)//30 is the image height
	{
		for (int x = 600; x < 792; x++) // 30 is image width
		{	
				pixelLeft->color = leftFrame[i]; 
				pixelLeft->x = x;
				pixelLeft->y = y;
	
				drawPixel(pixelLeft);
				i++;
		}
	}

	/* initialize a pixel */
	Pixel *pixelRight;
	pixelRight = malloc(sizeof(Pixel));
	int j=0;
	/* right frame */
	short int *rightFrame=(short int *) rightBorder.pixel_data;
	for (int y = 0; y < 720; y++)//30 is the image height
	{
		for (int x = 1688; x < 1880; x++) // 30 is image width
		{	
				pixelRight->color = rightFrame[j]; 
				pixelRight->x = x;
				pixelRight->y = y;
	
				drawPixel(pixelRight);
				j++;
		}
	}

	/* free pixel's allocated memory */
	free(pixelLeft);
	pixelLeft = NULL;
	free(pixelRight);
	pixelRight = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

int drawFrog(){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	short int *frogPtr=(short int *) frogFWD.pixel_data;
	
	//printf("Hola, %d",frogPtr[64]);
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;
	for (int y = 537; y < 600; y++)//30 is the image height; was 538,602
	{					// ^ changed from 601 to check something 
		for (int x = 1240; x < 1304; x++) // 30 is image width; was 1200,1264
		{	
				pixel->color = frogPtr[i+64];	//+64];
				pixel->x = x;
				pixel->y = y;
	
				if (pixel->color != 0){
					drawPixel(pixel);
				}
				i++;
				
		}
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	drawMoves();
	return 0;
}

int moveFrog(int buttonPressed){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;

	if(buttonPressed == 5){ // pressed UP
		
		short int *frogPtr=(short int *) frogFWD.pixel_data;
		lastPressedY = lastPressedY - 64;
		for (int y = lastPressedY; y < lastPressedY+63; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i+64]; 
					pixel->x = x;
					pixel->y = y;

					if (pixel->color != 0){
						drawPixel(pixel);
					}
					i++;
					
			}
		}
		
		movesTaken++;
		//playSound(hop); // plays hop sound; FIX: currently pauses inputs to process!
		
	} else if(buttonPressed == 6){ // pressed DOWN
		short int *frogPtr=(short int *) frogBWD.pixel_data;
		
		if(lastPressedY != 537){	// was 538
			lastPressedY = lastPressedY + 64;
			movesTaken++;
		}
		
		//lastPressedY = lastPressedY + 64;
		
		for (int y = lastPressedY; y < lastPressedY+63; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i+64]; 
					pixel->x = x;
					pixel->y = y;
		
					if (pixel->color != 0){
						drawPixel(pixel);
					}
					i++;
					
			}
		}
	} else if(buttonPressed == 7){ // pressed LEFT

		short int *frogPtr=(short int *) frogLWD.pixel_data;

		if(lastPressedX != 792){	// was 816, then 855 (hIndex was 13, now 14)
			lastPressedX = lastPressedX - 64;
			movesTaken++;
		}

		for (int y = lastPressedY; y < lastPressedY+63; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i+64]; 
					pixel->x = x;
					pixel->y = y;
		
					if (pixel->color != 0){
						drawPixel(pixel);
					}
					i++;
					
			}
		}
		
	} else{ // pressed RIGHT

		short int *frogPtr=(short int *) frogRWD.pixel_data;

		if(lastPressedX != 1624){	// was 1584 (hIndex was 13, now 14)
			lastPressedX = lastPressedX + 64;
			movesTaken++;
		}

		for (int y = lastPressedY; y < lastPressedY+63; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i+64]; 
					pixel->x = x;
					pixel->y = y;
		
					if (pixel->color != 0){
						drawPixel(pixel);
					}
					i++;
					
			}
		}
	}	
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	drawMoves();
	return 0;
}

int drawMoves(){

	int num = movesLeft - movesTaken;
	int mod = num % 10;
	num = num / 10;
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	Pixel *pixel2;
	pixel2 = malloc(sizeof(Pixel));
	unsigned int quarter,byte,word;

	
	if(num > 0){

		short int *numberPtr;
		numberPtr=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 720; y++)
		{					
			for (int x = 1368; x < 1399; x++) 
			{	
				pixel->color = numberPtr[(x-1368)+(y-665)*320+(32*num)];
				pixel->x = x;
				pixel->y = y;

				drawPixel(pixel);
			}
		}

		short int *numberPtr2;
		numberPtr2=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 720; y++)
		{					
			for (int x = 1400; x < 1431; x++) 
			{	
				pixel->color = numberPtr2[(x-1400)+(y-665)*320+(32*mod)];
				pixel->x = x;
				pixel->y = y;
	
				drawPixel(pixel);
			}
		}

	} else if(num <= 0){

		short int *numberPtr;
		numberPtr=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 720; y++)
		{					
			for (int x = 1368; x < 1399; x++) 
			{	
				pixel->color = numberPtr[(x-1368)+(y-665)*320+(32*mod)];
				pixel->x = x;
				pixel->y = y;
	
				drawPixel(pixel);
			}
		}
	} 

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	free(pixel2);
	pixel2 = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}



/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}


int drawBaddie(int lane, int level, int offset){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	short int *baddiePtr;
	
	if (level == 1) {
		baddiePtr=(short int *) lvlOne_Obs.pixel_data;
	}
	else if (level == 2) {
		baddiePtr=(short int *) lvlTwo_Obs.pixel_data;
	}
	else if (level == 3) {
		baddiePtr=(short int *) lvlThree_Obs.pixel_data;
	}
	else if (level == 4) {	// brain is mush, but leaving as not "else" in case something funky happens with the level value
		baddiePtr=(short int *) lvlFour_Obs.pixel_data;
	}
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;
	for (int y = 474; y < 537; y++)//30 is the image height; was 538,602
	{					// ^ was 538, but changed to avoid error
		for (int x = 1112; x < 1178; x++) // 30 is image width; was 1200,1264
		{	
				pixel->color = baddiePtr[(x-1112)+(y-473)*384];
				pixel->x = x;
				pixel->y = y;
	
				if (pixel->color != 0){
					drawPixel(pixel);
				}
				i++;
				
		}
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}
