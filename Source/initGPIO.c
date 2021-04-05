/*
CPSC 359 Project (Part 1)
Winter 2021

Lecturer: Dr. Jalal Kawash

Authors: Paul Serafini (ID# 30077288), Sahil Brar (ID# 30021440)

Assignment Objective(s):
Create a device driver for the SNES controller.

 * Authors' Note: Per README, ensure makefile is in the project folder,
 * that initGPIO files and this file are in a folder named "source",
 * with that an empty folder named "build" within the project folder

 * Authors' Note: We were permitted use of this file by a TA, being
 * taken from the GPIO-V1.1 folder uploaded under "Project" on d2l.

*/


#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "initGPIO.h"

unsigned int *getGPIOPtr(void)
{
	int fdgpio = open("/dev/gpiomem", O_RDWR);
	
	if (fdgpio <0) {
		printf("unable to open");
	}
	
	unsigned int *gpioPtr = (unsigned int *)mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fdgpio,0);
	
	return gpioPtr;
}
