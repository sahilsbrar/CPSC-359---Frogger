#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "framebuffer.h"
#include "drawImage.h"
#include "backgroundImg.h"
#include "frogFWD.h"
#include "frogBWD.h"
#include "frogLWD.h"
#include "frogRWD.h"


/* Definitions */

struct fbs framebufferstruct;

int lastPressedX = 1200;
int lastPressedY = 464;


/* main function */
int drawBack(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	short int *background=(short int *) backgroundImg.pixel_data;
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;
	unsigned int quarter,byte,word;
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
	for (int y = 464; y < 528; y++)//30 is the image height
	{
		for (int x = 1200; x < 1264; x++) // 30 is image width
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
	} else if(buttonPressed == 6){ // pressed DOWN
		short int *frogPtr=(short int *) frogBWD.pixel_data;
		lastPressedY = lastPressedY + 64;
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
		lastPressedX = lastPressedX - 64;
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
		lastPressedX = lastPressedX + 64;
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

