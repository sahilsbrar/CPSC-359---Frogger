#ifndef drawImage
#define drawImage

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

void drawPixel(Pixel *pixel);
int drawBack();
int drawFrog();
int moveFrog();
#endif