#ifndef _SPRITE_H					
#define _SPRITE_H					

#include "main.h"		
									
// These are are the states of the sprite							
#define NORTH 0						
#define EAST  1						
#define SOUTH 2					
#define WEST  3					

struct SPRITE {						// Sprite structure for animation
	HBITMAP hBitmap;				// Associated bitmap
	POINT position;					// Position of the sprite
	POINT speed;					// The x and y speed of the sprite
	int currentFrame;				// Current frame of animation
	int lastFrame;					// Amount of bitmaps that the sprite animates
	int state;						// Direction of the sprite
	int width;						// Width of one of the frames of animation
	int height;						// Height of one of the frames of animation
};

// This loads a sprite and reads in the bitmap associated with it
void LoadSprite(SPRITE *pSprite, LPSTR szFileName, int width, int height, int lastFrame);
// This returns true if we should animate the next frame depending on the frame rate
BOOL AnimateNextFrame(int desiredFrameRate);
// Sets a sprites X and Y position
void SetSpritePosition(SPRITE *pSprite, int x, int y);
// Sets the state of the animation (WEST, EAST, etc..)
void SetSpriteState(SPRITE *pSprite, int whichState);
// Sets the sprites x and y speed in pixels
void SetSpriteSpeed(SPRITE *pSprite, int xSpeed, int ySpeed);
// Animates and displays the current bitmap
void AnimateSprite(BUFFER *pBuffer, SPRITE *pSprite, double radians, int angle);
// Calculates the frames per second
void CalculateFrameRate(HWND hWnd);
// Moves the sprite depending on its state and speed
void MoveSprite(SPRITE *pSprite);
// Erases the sprite with a portion of the background
void EraseSprite(BUFFER *pBuffer, SPRITE *pSprite, HBITMAP hBackground);
// Frees a sprite that is passed in
void FreeSprite(SPRITE *pSprite);

#endif
