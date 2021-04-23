#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

typedef struct {
    int lives;
    int level;
    int score;
    int movesLeft;
	int movesTaken;
	int lastPressedX;
	int lastPressedY;
} gs;		// game state

//int timeLeft;
double timeLeft;

//int lives;
//int level;
//int score;
int laneOffsets[5];
double speedModifier;
int laneOccupancy[155];
bool collided;



/* Functions */
void drawPixel(Pixel *pixel);
bool getStart();
bool getQuit();
void getStatus();
void resetGame();
void frogDied();
int updateBoard();
int getOption();
int clear();
int drawOutCome();
int drawPause();
int drawValPack();
void checkClaim();
int drawMainMenu();
int drawGameScreen();
int drawFrames();
int drawFrog();
int moveFrog();
int drawMoves();
int drawScore();
int drawTimer();
int drawLanes();
int updateLaneOffsets();	// updates the offsets for the lanes; to be clock-synchronized
int drawDeaths();
//bool checkCollision();		// checks if our dear froggy has collided with an obstacle!
#endif
