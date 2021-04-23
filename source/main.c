/*
CPSC 359 Project (Part 2)
Winter 2021

Lecturer: Dr. Jalal Kawash

Authors: Paul Serafini (ID# 30077288), Sahil Brar (ID# 30021440)

Assignment Objective(s):
Create a Frogger game on the Raspberry Pi which works in tandem with
our previous implementation of a device driver for the SNES controller.

TO COMPILE & RUN IN LINUX TERMINAL, USE:

 make
 ./myProg

 * Authors' Note: Per README, ensure makefile is in the project folder,
 * that initGPIO files and this file are in a folder named "source",
 * with that an empty folder named "build" within the project folder

*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wiringPi.h>
#include <unistd.h>
#include <stdbool.h>
#include "initGPIO.h"
#include "drawImage.h"
#include <time.h>
#include <pthread.h>


/*  Declarations  */


#define CLK 11  // GPIO Pin 11 for CLOCK
#define LAT 9   // GPIO Pin 9 for LATCH
#define DAT 10  // GPIO Pin 10 for DATA

bool startBool = false;
bool quitBool = false;
bool paused = false;
bool gameOver = true;
bool timeUp = false;

int movF = 0;    // which direction is the froggie moving in?!?

/* Clock Thread */

void *clockie(void *id);

/*  Functions  */


// *** init_GPIO() ***
// Parameter(s): an int g, the line to be initialized; an int func, a function code corresponding to either
//               input (000) or output (001); an int pointer, pointing to the base address of our GPIO
// Return Value: None
// Use: initializes CLK, LAT, DAT lines as either input or output, according to passed function code (func)

void init_GPIO(int g, int func, unsigned int *gpio){
    if(func == 001){
        if(g == LAT){
            gpio[0] = ((gpio[0]& ~(0b111<<27)) | (0b001<<27));
        } else{
            gpio[1] = ((gpio[1]& ~(0b111<<3)) | (0b001<<3));
        }

    } else{
        if(g == LAT){
            gpio[0] = ((gpio[0]& ~(0b111<<27)) | (0b000<<27));
        } else if(g == CLK){
            gpio[1] = ((gpio[1]& ~(0b111<<3)) | (0b000<<3));
        } else{
            gpio[1] = ((gpio[1]& ~(0b111<<0)) | (0b000<<0));
        }
    }
}


// *** write_Latch() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: None
// Use: writes a bit to the gpio latch line

void write_Latch(unsigned int *gpio){
    gpio[7] = 1<<LAT;
}


// *** clear_Latch() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: None
// Use: clears the gpio latch line

void clear_Latch(unsigned int *gpio){
    gpio[10] = 1<<LAT;
}


// *** write_Clock() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: None
// Use: writes a bit to the gpio clock line

void write_Clock(unsigned int *gpio){
    gpio[7] = 1<<CLK;
}


// *** clear_Clock() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: None
// Use: clears the gpio clock line

void clear_Clock(unsigned int *gpio){
    gpio[10] = 1<<CLK;
}


// *** read_Data() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: int
// Use: reads the data register to obtain button sample

int read_Data(unsigned int *gpio){
    return(gpio[13]&(1<<DAT));
}


// *** wait() ***
// Parameter(s): an int, the number of microseconds our program waits
// Return Value: None
// Use: has our program wait for the passed number of microseconds

void wait(int time){
    delayMicroseconds(time);
}


// *** passInput() ***
// Parameter(s): an int, signifying the message to be printed; an int
//               array, each index i : [1,16] corresponding to current
//               pressed-state of button i (0 is a dummy, 13-16 unused)
// Return Value: None
// Use: passes controller input from user to game logic/clockie function below

void passInput(int message, int buttons[]){
    //char labels[17][20] = {"","B","Y","SELECT","START","Joy-pad UP","Joy-pad DOWN","Joy-pad LEFT","Joy-pad RIGHT","A","X","LEFT","RIGHT","","","",""};
                                                                        // Array indices 1 - 12 are the labels for each of the SNES controller buttons
                                                                        
    if(message == 0){                                                   // Print start message
        // printf("Created by Sahil Brar and Paul Serafini\n");
        // printf("\nPlease press a button...\n");
    } else if(message == 1 && buttons[4] != 0){                         // If button has been pressed; "message == 1" for case of future functionality

        for(int i = 1; i <= 16; i++){                                   // Goes up to 16 to fit SNES controller protocol, noting 13-16 to be undefined

            if(buttons[i] == 0 && i != 4 && i < 13){                    

                movF = 0;

                int value = getOption();
                if(value == 3){ // if winner

                    if(i != 0){ // pass controller input to go back to home screen
                        movF = i;
                    }
                    
                }else if(value == 4){ // if loser
                    
                    if(i != 0){ // pass contoller input to go back to home screen
                        movF = i; 
                    }

                }else if(startBool == false){ // on main menu, passes input for cycling options/selecting options
                    movF = i;

                }else if(paused == true){ // if on pause menu, passes controller input for cycling/selecting options
                    movF = i;
                    
                }else if(i >= 5 && i <= 8){ // pass controls of frog movement
                    //move the frog
                    movF = i;  
                } 
            }
        }
    } 
}


// *** read_SNES() ***
// Parameter(s): an int pointer, to the base address of our GPIO
// Return Value: None
// Use: reads our SNES controller, prompting the user
//      for a button press and afterwards printing it

void read_SNES(unsigned int *gpio){
    int buttons[17] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};              // An array representing buttons (first index is a dummy to make indexing easy)
    
    int pressed[17] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};              // An array marking buttons pressed just prior

    init_GPIO(CLK, 000, gpio);
    init_GPIO(CLK, 001, gpio);                                          // This and immediately above sets CLK to be output
    init_GPIO(LAT, 000, gpio);
    init_GPIO(LAT, 001, gpio);                                          // This and immediately above sets LAT to be output
    init_GPIO(DAT, 000, gpio);                                          // This sets DAT to be input
    
    passInput(0, buttons);                                          // Prints start message; used here (instead of in main function) to reduce cost
    
    
    /* Start Clock Thread Here */
    pthread_t tid100;
    pthread_create(&tid100,NULL,clockie,"100"); // Joined at bottom of this function


    //bool status = false;                                              // For future functionality (dealing with START menu, game states, and whatnot)
    bool change = false;                                                // Marks if change has occured between sequential presses
    int i = 1;
    while(!quitBool){                                            // Next 4 lines and for-loop derived from "Video - RPi 2 of 3" slide 20, on d2l
        write_Clock(gpio);
        write_Latch(gpio);
        wait(12);
        clear_Latch(gpio);

        for(i = 1; i <= 16; i++){                                   // Branches through all 16 buttons (recall index 0 of buttons array is a dummy)
            buttons[i] = 1;
            wait(6);
            clear_Clock(gpio);
            wait(6);
            int button = read_Data(gpio);
            if(button == 0){
                write_Clock(gpio);
                if(pressed[i] == 1){                                // If this button was just previously not pressed 
                    buttons[i] = 0;                                 // Marks that button i was pressed
                    change = true;                                  // A new button has been pressed
                }
                //else if(held[i]) = 1;                             // Pseudo; similar may be able to deal with button holding in the future
                pressed[i] = 0;                                     // Marks that button i was just pressed (for reference next physical "frame")
            }
            else{
                pressed[i] = 1;                                     // Marks that button i was not just pressed (for reference next physical "frame")
            }
            
            write_Clock(gpio);
            wait(500);
        }
        wait(500);                                                  // Waits; used to curb error due too fast of a cycle speed
    
        if(change == true){                                         // At least one button has been pressed
            passInput(1, buttons);                              // 1 means print status according to which buttons have been pressed
            change = false;
        }
        
        if(buttons[4] == 0){                                        // START button has been pressed
            
            // wait(500);
            
            if(startBool == true){ // make sure not on main menu (so pause menu doesnt open)
                int check = getOption();
                if(check == 3){
                    //if on win screen and start pressed, pass input to move to main menu
                    movF = 5;
                    
                }else if(check == 4){
                    // if on lose screen and start pressed, pass input to move to main menu
                    movF = 5;

                }else if(paused == false){ // if not paused, pause
                    paused = true;    

                }else{ // already paused and hit START, resumes game
                    movF = 0;   // prevents movement in pause menu; North orientation upon resuming
                    paused = false; // unpause
                }
            }
        }
    }
    gameOver = true; // game is over
    
    pthread_join(tid100,NULL);     // Join the clock thread!
    
    // printf("Time left is: %f", timeLeft);  // for testing
}


void *clockie(void *id){
    
    while(quitBool == false){ // while game not quit

        while(startBool == false){ // while game not started (on main menu)

            if(movF == 5 || movF == 6){ // pressing up or down
                drawMainMenu(movF); // cycling between main menu option
                updateBoard();
                
            }else if(movF == 9){ // user made a selection (A BUTTON)

                startBool = getStart(); //start game
                quitBool = getQuit(); // quit
                
                if(startBool == true){ // start game
                    gameOver = false;
                    timeLeft = 39.99; // reset clock for a new round (dbl)
                    timeUp = false;
                    resetGame();
                    drawGameScreen(0);
                    drawLanes();
                    drawValPack();
                    drawFrog(1);
                    drawFrames();
                    updateBoard();
                }else if(quitBool == true){ // quit game
                    clear();
                    gameOver = true;
                    startBool = true;
                    timeLeft = 0;
                }
            }
        }
        
        timeUp = false; // time is not up
        resetGame(); // before starting game

        while(gameOver == false){   // while game isnt over

            wait(50000);   // wait 50k microseconds; roughly 20fps!!! <--------
            timeLeft -= 0.05;   // COMPARE THIS VALUE TO ABOVE MICROSECONDS
            // ^ NOTE THAT YOU *MUST* TAKE speedModifier INTO CONSIDERATION
            //   WHEN ALTERING THESE VALUES AND ADJUST ACCORDINGLY!!!
            
            // Value Pack #1
            if(timeLeft < 12){
                speedModifier += 0.008;
            }

            int value = getOption(); // returns which bool flag is positive
            if(value == 3){ // if winner
                drawOutCome();  // draw win screen
                updateBoard();
                bool exit = false;
                while(exit == false){
                    if(movF != 0){  // wait until user presses a button and go to main menu
                        drawMainMenu(5);
                        updateBoard();
                        startBool = false;
                        gameOver = true;
                        timeLeft = 0;
                        exit = true;
                    }
                }
                
            }else if(value == 4 || timeUp == true){ // if lost game
                drawGameScreen(0);   // update screen for all parts and show lose screen
                drawLanes();
                drawValPack();
                drawFrog(1);

                drawFrames();
            
                drawTimer();
                drawDeaths();
                
                drawOutCome();
                updateBoard();
                wait(1000);
                bool exit = false;
                while(exit == false){
                    if(movF != 0){  //wait until user presses a button and go to main menu
                        drawMainMenu(5);
                        updateBoard();
                        startBool = false;
                        gameOver = true;
                        timeLeft = 0;
                        exit = true;
                    }
                }
            } else if (paused == true){ // if paused

                while(paused == true){ // pause timer while on pause menu

                    if(movF == 5 || movF == 6){
                        drawPause(movF); // cycle between options (UP or DOWN pressed)
                        updateBoard();

                    } else if(movF == 9){ // if user selects option
                        int option = getOption(); // get option selected by user

                        if(option == 1){ // user quit to main menu
                            drawMainMenu(5);    //draw main menu
                            updateBoard();
                            startBool = false;
                            gameOver = true;
                            timeLeft = 0;
                            paused = false;

                        }else if(option == 2){ // user reset game
                            resetGame();
                            timeLeft = 39.99; // reset clock for a new round (dbl)
                            paused = false;
                            updateBoard();
                        }
                    }else{
                        drawPause(5); // cycle between options (UP or DOWN pressed)
                        updateBoard();
                    }
                }
            } else{ // game controls passed for frog movement

                if(movF != 0){ // if moved the frog, draw the move and update everything on screen
                    drawGameScreen(movF);
                    drawLanes();
                    updateLaneOffsets();
                    drawValPack();
                    moveFrog(movF);
                    movF = 0;
                } else {    // otherwise redraw everything and don't move the frog
                    drawGameScreen(0);
                    drawLanes();
                    updateLaneOffsets();
                    drawValPack();
                    drawFrog(1);    // could easily add more options to make this work for all 4 cardinal directions
                }
                
                // ACCOUNT FOR COLLISIONS!
                if(collided == true){
                    //score -= 7;
                    frogDied();
                    collided = false;
                }
                // RESET FROG AT BEGINNING OF LEVEL HERE!!!

                drawScore(1);
                drawFrames();
            
                drawTimer();
                drawDeaths();
                
                updateBoard();
            }
            
            // to sync with display timer
            if(timeLeft < 3){
                timeUp = true;
            }

            checkClaim(); // check if any value packs have been used
            getStatus(); // check for lose conditions
        }

        if(quitBool == false){ // if havent quit the game, display main menu
            resetGame();
            drawMainMenu(5);
            updateBoard();
            startBool = false;
            movF = 0;
        }
    }
    pthread_exit(0); // exit thread
}


/*  Main Function  */

// *** main() ***
// Parameter(s): None
// Return Value: int, included only needed to meet makefile needs
// Use: runs our program

int main(){
    resetGame();
    drawMainMenu(5); // draw main menu
    
    updateBoard();
    
    unsigned int *gpio = getGPIOPtr();                                  // Obtains the base GPIO address

    read_SNES(gpio);                                                    // Calls controller reading subroutine

    return 0;
}
