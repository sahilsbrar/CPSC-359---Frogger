#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

int level;
int laneOffsets[5];
double speedModifier;
int laneOccupancy[155];

void drawPixel(Pixel *pixel);
bool getStart();
bool getQuit();
void resetGame();
int getOption();
int clear();
int drawOutCome();
int drawPause();
int drawMainMenu();
int drawGameScreen();
int drawFrames();
int drawFrog();
int moveFrog();
int drawMoves();
int drawBaddie();
int drawLanes();
int updateLaneOffsets();	// updates the offsets for the lanes; to be clock-synchronized
#endif
