#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

//int timeLeft;
double timeLeft;


int level;
int score;
int laneOffsets[5];
double speedModifier;
int laneOccupancy[155];



/* Functions */
void drawPixel(Pixel *pixel);
bool getStart();
bool getQuit();
void resetGame();
int getOption();
int clear();
int drawPause();
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
#endif
