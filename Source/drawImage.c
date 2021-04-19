#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "drawImage.h"
#include "lvl1.h"
#include "lvl2.h"
#include "lvl3.h"
#include "frogFWD.h"
#include "frogBWD.h"
#include "frogLWD.h"
#include "frogRWD.h"
#include <string.h>	//sounds


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
int lastPressedX = 1239;	// was 1200 (offset by +39)
int lastPressedY = 535;		// was 538 (offset by -3)


/* main function */
int drawBack(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	if(lastPressedY <= 154 && buttonPressed == 5){
		level = level + 1;
		lastPressedY = 599;	// was 602
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

	} else{
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
	}
	
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

int drawFrog(){
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	short int *frogPtr=(short int *) frogFWD.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;
	for (int y = 535; y < 599; y++)//30 is the image height; was 538,602
	{
		for (int x = 1239; x < 1303; x++) // 30 is image width; was 1200,1264
		{	
				pixel->color = frogPtr[i]; 
				pixel->x = x;
				pixel->y = y;
	
				drawPixel(pixel);
				i++;
				
		}
	}
	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
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
		for (int y = lastPressedY; y < lastPressedY+64; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
		
		//playSound(hop); // plays hop sound; FIX: currently pauses inputs to process!
		
	} else if(buttonPressed == 6){ // pressed DOWN
		short int *frogPtr=(short int *) frogBWD.pixel_data;
		
		if(lastPressedY != 535){	// was 538
			lastPressedY = lastPressedY + 64;
		}
		
		//lastPressedY = lastPressedY + 64;
		
		for (int y = lastPressedY; y < lastPressedY+64; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
	} else if(buttonPressed == 7){ // pressed LEFT

		short int *frogPtr=(short int *) frogLWD.pixel_data;

		if(lastPressedX != 791){	// was 816, then 855 (hIndex was 13, now 14)
			lastPressedX = lastPressedX - 64;
		}

		for (int y = lastPressedY; y < lastPressedY+64; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i]; 
					pixel->x = x;
					pixel->y = y;
		
					drawPixel(pixel);
					i++;
					
			}
		}
		
	} else{ // pressed RIGHT

		short int *frogPtr=(short int *) frogRWD.pixel_data;

		if(lastPressedX != 1623){	// was 1584 (hIndex was 13, now 14)
			lastPressedX = lastPressedX + 64;
		}

		for (int y = lastPressedY; y < lastPressedY+64; y++)//30 is the image height
		{
			for (int x = lastPressedX; x < lastPressedX+64; x++) // 30 is image width
			{	
					pixel->color = frogPtr[i]; 
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



/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	*((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
}

