#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <string.h>	//sounds
#include "framebuffer.h"
#include "drawImage.h"
#include <time.h>

#include "lostLife.h" 		// lostLife.pixel_data
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
#include "pauseResume.h"	// pauseResume.pixel_data -> should be restart but oops
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

int laneOffsets[5] = {25,35,15,0,10};				// pixels offset
int laneIndices[5] = {0,30,60,90,120};
double laneSpeeds[5] = {8.0,-12.0,10.0,-9.0,8.0};
double speedModifier = 3.0;

int laneOccupancy[155] = {0,1,1,1,0,0,0,0,1,1,0,0,0,1,0,1,0,0,0,1,1,0,0,0,0,1,1,0,1,0,0,0,1,1,1,1,0,0,0,1,1,0,0,0,1,1,0,0,1,1,1,1,0,0,0,0,1,1,0,1,1,0,0,0,1,1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,1,1,0,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0};

// Variables -> possible struct here?
int level = 1;
int score = 0;
int movesLeft = 75;
int movesTaken = 0;
int lastPressedX = 640;	// was 1200 (offset by +39)
int lastPressedY = 537;	// was 538 (offset by -3)
bool startGame = false;
bool quitGame = false;
bool goMain = false;
bool reset = false;
bool winner = false;
bool loser = false;
// int score = 157;
bool browniePts = false;
double timeLeft = 39.9;	// start with 40 seconds
//int timeLeft = 40;	// start with 40 seconds

short int colors[1500][1000];	// making much bigger than 1280*720 incase of OOB reference

int updateBoard(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));

	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 0; x < 1280; x++){ // 1280 is image width

			pixel->color = colors[x][y]; 
			pixel->x = x;
			pixel->y = y;
			
			if(pixel->color != 0){
				drawPixel(pixel);
			}
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

void resetGame(){
	level = 1;
	movesLeft = 75;
	movesTaken = 0;
	score = 0;
	lastPressedX = 640;	// was 1200 (offset by +39)
	lastPressedY = 537;		// was 538 (offset by -1)
	startGame = false;
	quitGame = false;
	goMain = false;
	reset = false;
	winner = false;
	loser = false;
	// maybe can reset time here, but believe presently needless
}

int getOption(){
	if(winner == true){
		return 3;
	}else if(loser == true){
		return 4;
	}else if(goMain == true){
		return 1;
	}else if(reset == true){
		return 2;
	}else{
		return 0;
	}
}
// called by main to see if game is started
bool getStart(){
	return startGame;
}

// called by main to see if game is quit
bool getQuit(){
	return quitGame;
}

// clears screen
int clear(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));

	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 0; x < 1280; x++){ // 1280 is image width
			colors[x][y] = 0;
			//pixel->color = 0; 
			//pixel->x = x;
			//pixel->y = y;

			//drawPixel(pixel);
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

// Draws Main Menu Screen
int drawMainMenu(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i = 0;

	short int *background;

	if(buttonPressed == 5){ // If user attempting to move to start game option
		startGame = true; // if user hits A BUTTON
		quitGame = false;

		background=(short int *) startImg.pixel_data;
	}else{ //user is moving towards Quit game text
		startGame = false;
		quitGame = true;

		background=(short int *) quitImg.pixel_data;
	}

	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 0; x < 1280; x++){ // 1280 is image width
			colors[x][y] = background[i];
			//pixel->color = background[i]; 
			//pixel->x = x;
			//pixel->y = y;

			//drawPixel(pixel);
			i++;
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

// Draw the actual game field
int drawGameScreen(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	if(lastPressedY <= 155 && buttonPressed == 5 && level != 4){ // checking if moving up to next level
		level = level + 1;
		lastPressedY = 601;
	}

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;

	short int *background;

	if(level == 1){
		background=(short int *) lvl_one.pixel_data;

	} else if(level == 2){
		background=(short int *) lvl_two.pixel_data;

	} else if(level == 3){
		background=(short int *) lvl_three.pixel_data;

	}else{
		background=(short int *) lvl_four.pixel_data;
	}

	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 0; x < 1280; x++){ // 1280 is image width	
			colors[x][y] = background[i];
			//pixel->color = background[i]; 
			//pixel->x = x;
			//pixel->y = y;

			//drawPixel(pixel);
			i++;
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);

	return 0;
}

// draws frames on side for overflow of obstacles
int drawFrames(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixelLeft;
	pixelLeft = malloc(sizeof(Pixel));
	int i=0;

	/* left frame */
	short int *leftFrame=(short int *) leftBorder.pixel_data;
	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 0; x < 192; x++){ // 192 is image width
				colors[x][y] = leftFrame[i];
				//pixelLeft->color = leftFrame[i]; 
				//pixelLeft->x = x;
				//pixelLeft->y = y;
	
				//drawPixel(pixelLeft);
				i++;
		}
	}

	/* initialize a pixel */
	Pixel *pixelRight;
	pixelRight = malloc(sizeof(Pixel));
	int j=0;

	/* right frame */
	short int *rightFrame=(short int *) rightBorder.pixel_data;
	for (int y = 0; y < 720; y++){ // 720 is the image height
		for (int x = 1088; x < 1280; x++){ // 192 is image width
				colors[x][y] = rightFrame[j];
				//pixelRight->color = rightFrame[j]; 
				//pixelRight->x = x;
				//pixelRight->y = y;
	
				//drawPixel(pixelRight);
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

// Draws Win/Lose Screens
int drawOutCome(){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;

	short int *outcomePtr;

	if(winner == true){ // win screen
		outcomePtr=(short int *) winScreen.pixel_data; // dispay win message

		for (int y = 200; y < 520; y++){ // 320 is the image height
			for (int x = 320; x < 960; x++){ // 640 is image width

				pixel->color = outcomePtr[i]; 
				pixel->x = x;
				pixel->y = y;

				drawPixel(pixel);
				i++;
			}
		}
	} else{ // lose screen
		outcomePtr=(short int *) loseScreen.pixel_data; // dispay lose message

		for (int y = 200; y < 520; y++){ // 320 is the image height
			for (int x = 320; x < 960; x++){ // 640 is image width

				pixel->color = outcomePtr[i]; 
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
	drawScore(2);
	return 0;
}

// Draws pause
int drawPause(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;

	short int *pMenu;

	if(buttonPressed == 5){ // user on reset game option
		reset = true;
		goMain = false;
		pMenu=(short int *) pauseResume.pixel_data; // dispay reset game option selected
	}else{ //user is moving towards quit to main menu option
		reset = false;
		goMain = true;
		pMenu=(short int *) pauseQuit.pixel_data; // display quit option selected
	}

	for (int y = 200; y < 520; y++){ // 320 is the image height
		for (int x = 320; x < 960; x++){ // 640 is image width
			colors[x][y] = pMenu[i];
			//pixel->color = pMenu[i]; 
			//pixel->x = x;
			//pixel->y = y;

			//drawPixel(pixel);
			i++;
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

// Draws frog in initial position, or when coming down a level
int drawFrog(int direction){

	if(lastPressedY < 100){	// is at the end!
		return 0;	// don't print dat froggo
	}

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	short int *frogPtr;

	if(direction == 1){ // initial position
		frogPtr=(short int *) frogFWD.pixel_data;
	}else{ // coming down a level
		frogPtr=(short int *) frogBWD.pixel_data;
	}
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;

	for (int y = lastPressedY; y < lastPressedY + 63; y++){ // 64 is the image height
		for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width
			//colors[x][y] = frogPtr[i+64];
			
			//pixel->color = frogPtr[i+64];
			//pixel->x = x;
			//pixel->y = y;

			if (frogPtr[i+64] != 0){ // dont print black pixels (for around frog)
				//drawPixel(pixel);
				colors[x][y] = frogPtr[i+64];
				;
			}
			i++;
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	drawScore(1);
	drawMoves();
	return 0;
}

// draw frog in new position based on controller input
int moveFrog(int buttonPressed){

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	int i=0;

	if(buttonPressed == 5){ // pressed UP
		if(lastPressedY <= 155 && level == 4){	// Restricting level 4 top movements through gate only (feature)
			if(lastPressedX == 576 || lastPressedX == 640){
				lastPressedY = lastPressedY - 64;
				movesTaken++;
				int move = movesLeft - movesTaken;

				if(browniePts == false){	// MAKES SURE THAT WINNING PTS GAIN ONLY HAPPENS ONCE!!
					score += 100;	// good for you
					browniePts = true;	// such wow
				}

				if(move >= 0){ //winner if moves are left
					winner = true;
					drawScore(1);
					drawMoves(); // update moves left on bottom on screen
					drawOutCome();
					return 0;

				}else{ // no moves to move up, loserrrrr
					loser = true;
					drawScore(1);
					drawMoves(); // update moves left on bottom on screen
					drawOutCome();
					return 0;
				}
				movesTaken++;	// increment moves taken
				
			}else{
				lastPressedY = lastPressedY; // cannot move due to bounds
			}
		}else{ // regular up movements
			lastPressedY = lastPressedY - 64;
			movesTaken++;	// increment moves taken
			score += 10;	// you deserve this so much
		}

		short int *frogPtr=(short int *) frogFWD.pixel_data; // set forward movement image
		
		for (int y = lastPressedY; y < lastPressedY+63; y++){ //64 is the image height
			for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width
				//colors[x][y] = frogPtr[i+64];
				
				//pixel->color = frogPtr[i+64]; 
				//pixel->x = x;
				//pixel->y = y;

				if (frogPtr[i+64] != 0){ // dont print black pixels (for around frog)
					//drawPixel(pixel);
					colors[x][y] = frogPtr[i+64];
					;
				}
				i++;	
			}
		}
		
		//playSound(hop); // plays hop sound; FIX: currently pauses inputs to process!
		
	} else if(buttonPressed == 6){ // pressed DOWN

		short int *frogPtr=(short int *) frogBWD.pixel_data; // set downward movement image
		
		if(lastPressedY == 537 && level != 1){ // If the user is moving back a level
			level--; // go back a level
			lastPressedY = 153; // set appropriate location
			movesTaken++; // increment moves taken
			drawGameScreen(6); // redraw game screen based on moving back a level
			drawLanes();
			drawFrames(); // draw frames for obstacles overflow
			drawFrog(2); // redraw frog facing down

		} else if(lastPressedY != 537){	// checking if user is moving down (as long as not on lvl1 and on bottom row)

			lastPressedY = lastPressedY + 64; // move down
			movesTaken++; // increment moves taken

			for (int y = lastPressedY; y < lastPressedY+63; y++){ //64 is the image height
				for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width	
					//colors[x][y] = frogPtr[i+64];
					//pixel->color = frogPtr[i+64]; 
					//pixel->x = x;
					//pixel->y = y;
		
					if (frogPtr[i+64] != 0){ // dont print black pixels (for around the frog)
						//drawPixel(pixel);
						colors[x][y] = frogPtr[i+64];
						;
					}
					i++;	
				}
			}

		} else{ // keep user in same spot, dont increment moves taken
			for (int y = lastPressedY; y < lastPressedY+63; y++){ // 64 is the image height
				for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width
						//colors[x][y] = frogPtr[i+64];
						
						//pixel->color = frogPtr[i+64]; 
						//pixel->x = x;
						//pixel->y = y;
			
						if (frogPtr[i+64] != 0){ // dont print black pixels (around frog)
							//drawPixel(pixel);
							colors[x][y] = frogPtr[i+64];
							;
						}
						i++;
				}
			}
		}
		
	} else if(buttonPressed == 7){ // pressed LEFT

		short int *frogPtr=(short int *) frogLWD.pixel_data; // set image to let movement

		// checking if user is trying to move on gate (level 4) from left, or off gate (feature to make win exit)
		if(lastPressedY <= 155 && (lastPressedX == 704 || lastPressedX == 576)){ 

			if(level != 4){ //if not on level 4, make the move like normal
				lastPressedX = lastPressedX - 64; // move frog position
				movesTaken++; // increment moves taken
			}

		} else if(lastPressedX != 192){	// make sure player doesnt move left out of bounds
			lastPressedX = lastPressedX - 64; // move player 
			movesTaken++; // increment moves taken
		}

		for (int y = lastPressedY; y < lastPressedY+63; y++){ // 64 is the image height
			for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width
					//colors[x][y] = frogPtr[i+64];
					
					//pixel->color = frogPtr[i+64]; 
					//pixel->x = x;
					//pixel->y = y;
		
					if (frogPtr[i+64] != 0){ // dont print black pixels around frog
						//drawPixel(pixel);
						colors[x][y] = frogPtr[i+64];
						;
					}
					i++;
			}
		}
		
	} else{ // pressed RIGHT

		short int *frogPtr=(short int *) frogRWD.pixel_data; // use right moving image
		
		// checking if user is trying to move onto/off draw bridge (lvl4) from the right (feature for win condition)
		if(lastPressedY <= 155 && (lastPressedX == 512 || lastPressedX == 640)){
			if(level != 4){ // if not on level 4
				lastPressedX = lastPressedX + 64; // move like normal
				movesTaken++; // increment moves taken
			}
		} else if(lastPressedX != 1024){ // otherwise check and prevent user from moving out of bounds (right side)
			lastPressedX = lastPressedX + 64; // move frog
			movesTaken++; // increment moves taken
		}

		for (int y = lastPressedY; y < lastPressedY+63; y++){ //64 is the image height
			for (int x = lastPressedX; x < lastPressedX+64; x++){ // 64 is image width
					//colors[x][y] = frogPtr[i+64];
					
					//pixel->color = frogPtr[i+64]; 
					//pixel->x = x;
					//pixel->y = y;
		
					if (frogPtr[i+64] != 0){ // dont draw black pixels around frog
						//drawPixel(pixel);
						colors[x][y] = frogPtr[i+64];
						;
					}
					i++;
			}
		}
	}	

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	drawScore(1);
	drawMoves(); // update moves left on bottom on screen
	return 0;
}

// function to display score on screen
int drawScore(int location){

	int modH = score / 100;	// hundreds digit
	int modT = score / 10;	// tens digit
	int modO = score % 10;	// ones digit

	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));

	// TEST: use just the one point cuz why tf not if same file :(
	short int *numberPtr;
	numberPtr=(short int *) numbersImg.pixel_data;

	if(location == 1){
		// first digit
		for (int y = 666; y < 720; y++){					
			for (int x = 384; x < 415; x++) {

				if(numberPtr[(x-384)+(y-665)*320+(32*modH)] == -9340){
					colors[x][y] = numberPtr[(x-384)+(y-665)*320+(32*modH)];
				}
			}
		}

		// second digit
		for (int y = 666; y < 720; y++){					
			for (int x = 416; x < 447; x++){	

				if(numberPtr[(x-416)+(y-665)*320+(32*modT)] == -9340){
					colors[x][y] = numberPtr[(x-416)+(y-665)*320+(32*modT)];
				}
			}
		}

		// third digit
		for (int y = 666; y < 720; y++){					
			for (int x = 448; x < 479; x++){	

				if(numberPtr[(x-448)+(y-665)*320+(32*modO)] == -9340){
					colors[x][y] = numberPtr[(x-448)+(y-665)*320+(32*modO)];
				}
			}
		}

	}else{
		
		// first digit
		for (int y = 394; y < 449; y++){					
			for (int x = 640; x < 671; x++) {

				if(numberPtr[(x-640)+(y-393)*320+(32*modH)] == -9340){
					colors[x][y] = numberPtr[(x-640)+(y-393)*320+(32*modH)];
				}
			}
		}

		// second digit
		for (int y = 394; y < 449; y++){					
			for (int x = 672; x < 703; x++){

				if(numberPtr[(x-672)+(y-393)*320+(32*modT)] == -9340){
					colors[x][y] = numberPtr[(x-672)+(y-393)*320+(32*modT)];
				}
			}
		}

		// third digit
		for (int y = 394; y < 449; y++){					
			for (int x = 704; x < 735; x++){

				if(numberPtr[(x-704)+(y-393)*320+(32*modO)] == -9340){
					colors[x][y] = numberPtr[(x-704)+(y-393)*320+(32*modO)];
				}
			}
		}
		
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

// function to display moves remaining on screen
int drawMoves(){

	int num = movesLeft - movesTaken;
	int mod = num % 10; // second digit
	num = num / 10; // first digit
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));

	// TEST
	short int *numberPtr;
	numberPtr=(short int *) numbersImg.pixel_data;
	
	
	if(num > 0){ // if num isnt 0, means 2 digit number

		// first digit
					//short int *numberPtr;
					//numberPtr=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 720; y++){					
			for (int x = 768; x < 799; x++) {
				//colors[x][y] = numberPtr[(x-768)+(y-665)*320+(32*num)];
				
				//pixel->color = numberPtr[(x-768)+(y-665)*320+(32*num)];
				//pixel->x = x;
				//pixel->y = y;

				//if(pixel->color != 0){
					//drawPixel(pixel);
				//	;
				//}
				//if(pixel->color == -9340){
				if(numberPtr[(x-768)+(y-665)*320+(32*num)] == -9340){
					//drawPixel(pixel);
					colors[x][y] = numberPtr[(x-768)+(y-665)*320+(32*num)];

				}
			}
		}

		// second digit
					//short int *numberPtr2;
					//numberPtr2=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 720; y++)
		{					
			for (int x = 800; x < 831; x++) 
			{	
				//colors[x][y] = numberPtr[(x-800)+(y-665)*320+(32*mod)];
				
				//pixel->color = numberPtr[(x-800)+(y-665)*320+(32*mod)];
				// ^ WAS: pixel->color = numberPtr2
				//pixel->x = x;
				//pixel->y = y;
	
				//if(pixel->color == -9340){
				if(numberPtr[(x-800)+(y-665)*320+(32*mod)] == -9340){
					//drawPixel(pixel);
					colors[x][y] = numberPtr[(x-800)+(y-665)*320+(32*mod)];
					;
				}
			}
		}

	} else if(num <= 0){ // means only one digit

		// zero
					//short int *numberPtr;
					//numberPtr=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 729; y++){					
			for (int x = 768; x < 799; x++){	
				//colors[x][y] = numberPtr[(x-768)+(y-665)*320];				
				
				//pixel->color = numberPtr[(x-768)+(y-665)*320];
				//pixel->x = x;
				//pixel->y = y;
	
				//if(pixel->color == -9340){
				if(numberPtr[(x-768)+(y-665)*320] == -9340){
					//drawPixel(pixel);
					colors[x][y] = numberPtr[(x-768)+(y-665)*320];
					;
				}
			}
		}

		// second digit
					//short int *numberPtr2;
					//numberPtr2=(short int *) numbersImg.pixel_data;
		for (int y = 666; y < 729; y++)
		{					
			for (int x = 800; x < 831; x++) 
			{	
				//colors[x][y] = numberPtr[(x-800)+(y-665)*320+(32*mod)];
				
				//pixel->color = numberPtr[(x-800)+(y-665)*320+(32*mod)];
				// ^ WAS: pixel->color = numberPtr2
				//pixel->x = x;
				//pixel->y = y;
	
				//if(pixel->color == -9340){
				if(numberPtr[(x-800)+(y-665)*320+(32*mod)] == -9340){
					//drawPixel(pixel);
					colors[x][y] = numberPtr[(x-800)+(y-665)*320+(32*mod)];
					
				}
			}
		}
	} 

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

// /* Draw the Current Score */
// int drawScore(){

// 	int modH = score / 100;	// hundreds digit
// 	int modT = score / 10;	// tens digit
// 	int modO = score % 10;	// ones digit
	
// 	/* initialize + get FBS */
// 	framebufferstruct = initFbInfo();

// 	/* initialize a pixel */
// 	Pixel *pixel;
// 	pixel = malloc(sizeof(Pixel));

	
// 	// TEST: use just the one point cuz why tf not if same file :(
// 	short int *numberPtr;
// 	numberPtr=(short int *) numbersImg.pixel_data;

// 	// hundreds digit
// 				//short int *numberPtr;
// 				//numberPtr=(short int *) numbersImg.pixel_data;
// 	for (int y = 666; y < 729; y++){					
// 		for (int x = 384; x < 415; x++) {
// 				// ^ -384 from "Moves"; was 768, 799
// 			//colors[x][y] = numberPtr[(x-384)+(y-665)*320+(32*modH)];
			
// 			//pixel->color = numberPtr[(x-384)+(y-665)*320+(32*modH)];
			
// 			//pixel->x = x;
// 			//pixel->y = y;

// 			//if(pixel->color == -9340){
// 			if(numberPtr[(x-384)+(y-665)*320+(32*modH)] == -9340){
// 				//printf("Our colour is: %d!", pixel->color);	// TEST
// 				//drawPixel(pixel);
// 				colors[x][y] = numberPtr[(x-384)+(y-665)*320+(32*modH)];
// 			}
// 		}
// 	}

// 	// second digit
// 				//short int *numberPtr2;
// 				//numberPtr2=(short int *) numbersImg.pixel_data;
// 	for (int y = 666; y < 729; y++){					
// 		for (int x = 416; x < 447; x++) {
// 				// ^ -384 from "Moves"; was 800, 831	
// 			//colors[x][y + modH] = numberPtr[(x-416)+(y-665)*320+(32*modT)];
			
// 			//pixel->color = numberPtr[(x-416)+(y-665)*320+(32*modT)];
// 			// ^ WAS: pixel->color = numberPtr2[(x-416)+(y-665)*320+(32*modT)];
// 			//pixel->x = x;
// 			//pixel->y = y + modH;	// sometimes the thing goes up...?; adding "+ modH" as a band-aid

// 			//if(pixel->color == -9340){
// 			if(numberPtr[(x-416)+(y-665)*320+(32*modT)] == -9340){
// 				//printf("Our colour is: %d!", pixel->color);	// TEST
// 				//drawPixel(pixel);
// 				colors[x][y + modH] = numberPtr[(x-416)+(y-665)*320+(32*modT)];
// 				//colors[x][y] = numberPtr[(x-416)+(y-665)*320+(32*modT)];
// 			}
// 		}
// 	}
	
// 	// third digit
// 				//short int *numberPtr3;
// 				//numberPtr3=(short int *) numbersImg.pixel_data;
// 	for (int y = 666; y < 729; y++){					
// 		for (int x = 448; x < 479; x++) {
// 				// ^ -384 from "Moves"; +32 from 2nd digit's 416/447	
// 			//colors[x][y] = numberPtr[(x-448)+(y-665)*320+(32*modO)];
			
// 			//pixel->color = numberPtr[(x-448)+(y-665)*320+(32*modO)];
// 			// ^ WAS: pixel->color = numberPtr3[(x-416)+(y-665)*320+(32*modT)];
// 			//pixel->x = x;
// 			//pixel->y = y;

// 			//if(pixel->color == -9340){
// 			if(numberPtr[(x-448)+(y-665)*320+(32*modO)] == -9340){
// 				//printf("Our colour is: %d!", pixel->color);	// TEST
// 				//drawPixel(pixel);
// 				colors[x][y] = numberPtr[(x-448)+(y-665)*320+(32*modO)];
// 			}
// 		}
// 	}


// 	/* free pixel's allocated memory */
// 	free(pixel);
// 	pixel = NULL;
// 	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
// 	return 0;
// }

/* Draw the timer */
int drawTimer(){
	
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();

	/* initialize a pixel */
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	
	short int *timerPtr;
	timerPtr=(short int *) timerBar.pixel_data;
	for (int y = 602; y < 665; y++){	
			// ^ is one grid space above "Moves"; that at 666,729
		//for (int x = 768; x < 1087; x++) {
		for (int x = 768; x < 1087 - (40 - (int)(timeLeft))*8; x++) {
				// ^ same start as from "Moves"; 768, but 5*64=320 wide
			//colors[x + (40 - (int)(timeLeft))*8][y] = timerPtr[(x-768)+(y-602)*320 + (40 - (int)(timeLeft))*8];
			
			//pixel->color = timerPtr[(x-768)+(y-602)*320 + (40 - (int)(timeLeft))*8];
			//pixel->x = x + (40 - (int)(timeLeft))*8;	// offset for timeBar is shrinking
			//pixel->y = y;

			if(timerPtr[(x-768)+(y-601)*320 + (40 - (int)(timeLeft))*8] != 0){
				//printf("Our colour is: %d!", pixel->color);	// TEST
				//drawPixel(pixel);
				colors[x + (40 - (int)(timeLeft))*8][y] = timerPtr[(x-768)+(y-601)*320 + (40 - (int)(timeLeft))*8];
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

int drawLanes(){	// offset from 0-63?62?
	/* initialize + get FBS */
	framebufferstruct = initFbInfo();
	
	Pixel *pixel;
	pixel = malloc(sizeof(Pixel));
	
	for (int n = 1; n < 6; n++){				// for 5 lanes
		
		//printf("\n And now we have that n = %i\n",n);
		//printf("Lane offset is: %i .\n",laneOffsets[n-1]);
		//printf("Lane index is: %i .\n",laneIndices[n-1]);
		
		for (int q = 0; q < 16; q++){			// check 16 indices (12 b/c no cutting rn)
			
			if (laneOccupancy[(laneIndices[n-1] + q) % 155] == 1){
				
				int offset = laneOffsets[(n-1)] + q*64;	// 64 is per grid space
				int lane = n;
				
				if (lane == 2){
					q++;	// avoid printing overlapping, double-long obstacles
				}
				
				if (n < 3){
					--lane;	// to match indices for pixel colour in loop
				}
				
				int vertOff = 0;
				if (lane >= 2){
					vertOff = 64;	// to match indices for pixel colour in loop, only past double
				}
					
				short int *baddiePtr;
				
				int twoLongObs = 0;
				if (lane == 1){		// input is actually 2 before decrementation
					twoLongObs = 64;
				}
				
				if (level == 1) {
					baddiePtr=(short int *) lvlOne_Obs.pixel_data;
				}
				else if (level == 2) {
					baddiePtr=(short int *) lvlTwo_Obs.pixel_data;
				}
				else if (level == 3) {
					baddiePtr=(short int *) lvlThree_Obs.pixel_data;
				}
				else {
					baddiePtr=(short int *) lvlFour_Obs.pixel_data;
				}
				
				for (int y = 474 - lane*64; y < 537 - lane*64; y++)//30 is the image height; was 538,602
				{					
					for (int x = 168; x < (232 + twoLongObs); x++) // 30 is image width; was 1200,1264
					{	// ^ was 832/896; additionally -600 from 768/832
							//colors[x + offset][y + vertOff] = baddiePtr[(x-168)+(y-473+lane*64)*384 + lane*64];
						
							//pixel->color = baddiePtr[(x-168)+(y-473+lane*64)*384 + lane*64];
							//pixel->x = x + offset;	// x offset (passed) based upon 
							//pixel->y = y + vertOff;	// vertical offset required for lanes 3 - 5 to reclaim lane shift from double-obs
				
							if (baddiePtr[(x-168)+(y-473+lane*64)*384 + lane*64] != 0){
								//drawPixel(pixel);
								colors[x + offset][y + vertOff] = baddiePtr[(x-168)+(y-473+lane*64)*384 + lane*64];
							}
							
							
					}
				}	// end of individual pixel loop
				// printf("\n We made it!\n"); TEST
				
			}	// printed one index (q, if 1) of lane n
		}	// end of positional iteration
	}	// end of lane loop

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
	
	return 0;
}

int updateLaneOffsets(){
	
	//printf("Lane 1 Offset is: %i", laneOffsets[0]);	// TEST
	
	for (int n = 0; n < 5; n++){	// for 5 lanes
		
		if (level < 3){
			laneOffsets[n] += (int) laneSpeeds[n]*speedModifier;
		} else {
			laneOffsets[n] -= (int) laneSpeeds[n]*speedModifier;
		}
		
		if (laneOffsets[n] > 63){
			
			laneOffsets[n] = laneOffsets[n] % 64;
			
			--laneIndices[n];
			
			if (laneIndices[n] < 0){
				laneIndices[n] =+ 154;
			}
			
		}
		
		else if (laneOffsets[n] < -63){
			
			laneOffsets[n] = laneOffsets[n] % 64;
			
			laneIndices[n]++;
			laneIndices[n] = laneIndices[n] % 154;
		}
		
	}
	return 0;
}
