#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

void drawPixel(Pixel *pixel);
int drawGameScreen();
int drawFrames();
int drawFrog();
int moveFrog();
#endif