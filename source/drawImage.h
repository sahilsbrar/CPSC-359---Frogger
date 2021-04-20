#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

void drawPixel(Pixel *pixel);
bool getStart();
bool getQuit();
int drawMainMenu();
int drawGameScreen();
int drawFrames();
int drawFrog();
int moveFrog();
int drawBaddie();
#endif
