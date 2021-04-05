/*
CPSC 359 Project (Part 1)
Winter 2021

Lecturer: Dr. Jalal Kawash

Authors: Paul Serafini (ID# 30077288), Sahil Brar (ID# 30021440)

Assignment Objective(s):
Create a device driver for the SNES controller.

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



/*  Declarations  */


#define CLK 11  // GPIO Pin 11 for CLOCK
#define LAT 9   // GPIO Pin 9 for LATCH
#define DAT 10  // GPIO Pin 10 for DATA



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


// *** print_Message() ***
// Parameter(s): an int, signifying the message to be printed; an int
//               array, each index i : [1,16] corresponding to current
//               pressed-state of button i (0 is a dummy, 13-16 unused)
// Return Value: None
// Use: prints a message to the user, either calling for a button press,
//      displaying which button is pressed, or notifying the user that
//      the program is terminating

void print_Message(int message, int buttons[]){
    char labels[17][20] = {"","B","Y","SELECT","START","Joy-pad UP","Joy-pad DOWN","Joy-pad LEFT","Joy-pad RIGHT","A","X","LEFT","RIGHT","","","",""};
                                                                        // Array indices 1 - 12 are the labels for each of the SNES controller buttons
                                                                        
    if(message == 0){                                                   // Print start message
        printf("Created by Sahil Brar and Paul Serafini\n");
        printf("\nPlease press a button...\n");
    } else if(message == 1 && buttons[4] != 0){                         // If button has been pressed; "message == 1" for case of future functionality
        for(int i = 1; i <= 16; i++){                                   // Goes up to 16 to fit SNES controller protocol, noting 13-16 to be undefined
            if(buttons[i] == 0 && i != 4 && i < 13){                    
                printf("\nYou have pressed %s\n", labels[i]);           // Prints pressed button message with appropriate label
    
                if(i >= 5 && i <= 8){
                    //move the frog
                    drawBack();
                    moveFrog(i);
                }
            }
        }
        printf("\nPlease press a button...\n");                         // Here allows successive printing of buttons pressed on same physical "frame"
    } else{                                                             // If start button has been pressed; not explicit, other codes may be required
        printf("\nProgram is terminating...\n");
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
    
    print_Message(0, buttons);                                          // Prints start message; used here (instead of in main function) to reduce cost

    //bool status = false;                                              // For future functionality (dealing with START menu, game states, and whatnot)
    bool startButton = false;                                           // Marks press-state of startButton
    bool change = false;                                                // Marks if change has occured between sequential presses
    int i = 1;
    //while(!status){                                                   // For future functionality (dealing with START menu, game states, and whatnot)
        while(!startButton){                                            // Next 4 lines and for-loop derived from "Video - RPi 2 of 3" slide 20, on d2l
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
                print_Message(1, buttons);                              // 1 means print status according to which buttons have been pressed
                change = false;
            }
            
            if(buttons[4] == 0){                                        // START button has been pressed
                startButton = true;                                     // Stops the current while-loop
            }
        }
    //status = false;                                                   // For future functionality (dealing with START menu, game states, and whatnot)
    //}                                                                 // For future functionality (dealing with START menu, game states, and whatnot)
}



/*  Main Function  */


// *** main() ***
// Parameter(s): None
// Return Value: int, included only needed to meet makefile needs
// Use: runs our program

int main(){
    drawBack();
    drawFrog();
    unsigned int *gpio = getGPIOPtr();                                  // Obtains the base GPIO address
    
    /* IMMEDIATELY BELOW IS FOR POTENTIAL LATER FUNCTIONALITY
    int buttons[17] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};              // An array representing buttons (first index is a dummy to make indexing easy)
    print_Message(0, buttons);                                          // Prints start message; may be more appropriate to call here than in read_SNES
    */

    read_SNES(gpio);                                                    // Calls controller reading subroutine

    return 0;
}
