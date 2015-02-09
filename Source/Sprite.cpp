#include "main.h"										
#define MONSTER_WIDTH  72
#define MONSTER_HEIGHT 77
#define TREE_WIDTH 115
#define TREE_HEIGHT 129
#define CLOUD_WIDTH 115
#define CLOUD_HEIGHT 129

// Loads a bitmap then display parts of it to animate
void LoadSprite(SPRITE *pSprite, LPSTR szFileName, int width, int height, int lastFrame)
{
	char szBuffer[255];
	int i = 0;

	// Set the sprites bitmapCount so it knows when to start over when animating
	pSprite->lastFrame = lastFrame - 1;

	// Initialize the currentFrame to 0
	pSprite->currentFrame= 0;

	// Set the sprite animation to the first state
	SetSpriteState(pSprite, WEST);

	// Set the sprites speed to default
	SetSpriteSpeed(pSprite, 0, 0);

	// Call our load bitmap function and pass in the bitmap to load
	pSprite->hBitmap = LoadABitmap(szFileName);

	// If we didn't receive a valid HBITMAP back, it didn't find the bitmap name
	if(!pSprite->hBitmap) 
	{
		// Store the bitmap file name in a buffer with a sentence to display
		sprintf(szBuffer, "Can't find the bitmap: %s!", szFileName);

		// Display a dialog box that says the error message and file name
		MessageBox(NULL, szBuffer, "Error!", MB_OK);
			
		// Quit the program
		exit(0);
	}

	// Set the width and height of the sprite
	pSprite->width = width;
	pSprite->height = height;
}
// Sets the sprite's x and y position
void SetSpritePosition(SPRITE *pSprite, int x, int y)
{
	// Set the x and y position of the sprite to the x and y passed in
	pSprite->position.x = x;
	pSprite->position.y = y;
}

// Sets the sprite's state such as WEST, NORTH, etc...
void SetSpriteState(SPRITE *pSprite, int whichState)
{
	// Set the sprites state such as WEST, EAST, etc...
	pSprite->state = whichState;
}

// Sets the sprite's x and y speed (in pixels)
void SetSpriteSpeed(SPRITE *pSprite, int xSpeed, int ySpeed)
{
	// Set the X and Y speed of the sprite
	pSprite->speed.x = xSpeed;
	pSprite->speed.y = ySpeed;
}

// Returns TRUE every time we want to display the next 
// frame of animation.  Say we wanted 30 frames of animation
// per second, it would return TRUE 30 times every second.
BOOL AnimateNextFrame(int desiredFrameRate)
{
	static float lastTime = 0.0f;
	float elapsedTime = 0.0;

	// Get current time in seconds  (milliseconds * .001 = seconds)
    float currentTime = GetTickCount() * 0.001f; 

	// Get the elapsed time by subtracting the current time from the last time
	elapsedTime = currentTime - lastTime;

	// Check if the time since we last checked is over (1 second / framesPerSecond)
    if( elapsedTime > (1.0f / desiredFrameRate) )
    {
		// Reset the last time
        lastTime = currentTime;	

		// Return TRUE, to animate the next frame of animation
        return TRUE;
    }

	// We don't animate right now.
	return FALSE;
}

// Calculates the frames per second
void CalculateFrameRate(HWND hWnd)
{
	static float fFPS      = 0.0f;
    static float fLastTime = 0.0f;
    static DWORD dwFrames  = 0;
	char strFrameRate[55] = {0};

    // Keep track of the time lapse and frame count
    float fTime = GetTickCount() * 0.001f; // Convert time to seconds
    ++dwFrames;

    // If our current time minus the last recorded time is great than 1 second, start over
    if( fTime - fLastTime > 1.0f)
    {
		// Store the current time to test again
        fLastTime = fTime;

		// Store our Frames Per Second Into a buffer
		sprintf(strFrameRate, "Frames Per Second: %d", dwFrames);

		// Display the Frames Per Second in our window title
		SetWindowText(hWnd, strFrameRate);

		// Reset the frames
		dwFrames  = 0;
    }
}

// Displays the current frame of animation, then increases it. 
void AnimateSprite(BUFFER *pBuffer, SPRITE *pSprite, double radians, int angle)
{
	// This holds the rectangle coordinates we want to display from the bitmap
	RECT rPortion;
	// Gets the y value in which we want to start at.
	int yPos = 1;
	
	(pSprite->state * ((pSprite->lastFrame + 1) * pSprite->height));

	// Fill in the structure with coordinated we need to get from the bitmap
	rPortion.left = 1;
	rPortion.top =  yPos;
	rPortion.right = pSprite->width + 1;
	rPortion.bottom = yPos + pSprite->height;

	// Rotate bitmap - does not work yet
//	pSprite->hBitmap = GetRotatedBitmapNT( pSprite->hBitmap, radians, pBuffer->hdcBack, pSprite->position.x, pSprite->position.y); //destX, destY);

	// Display the currentBitmap indexed into the hBitmap array.
	DisplayTransparentBitmap(pBuffer, pSprite->hBitmap, 
						   pSprite->position.x, pSprite->position.y, rPortion);

	// Increment to the next frame
	pSprite->currentFrame++;
	// Check if we reached the end of the animation
	if(pSprite->currentFrame > pSprite->lastFrame)
	{
		// Reset the currentBitmap to the beginning
		pSprite->currentFrame = 0;
	}

}

// Moves the sprite on screen depending on it's direction and speed          
void MoveSprite(SPRITE *pSprite)
{
	int y;
	// Check the sprites direction
	switch(pSprite->state)
	{	
		case NORTH:
			// Move the sprite north
			y = pSprite->position.y - pSprite->speed.y;
			if (y < -TREE_HEIGHT)
				y = HEIGHT - pSprite->position.y;
			SetSpritePosition(pSprite, pSprite->position.x, y);

			break;
		case SOUTH:
			// Move the sprite south
			// Goes back...
			SetSpritePosition(pSprite, pSprite->position.x, (pSprite->position.y + pSprite->speed.y) % HEIGHT);
			break;
		case EAST: 
			// Move the sprite east
			// Goes back...
			SetSpritePosition(pSprite, (pSprite->position.x + pSprite->speed.x) % WIDTH, pSprite->position.y);
			break;
		case WEST:
			// Move the sprite west
			int x = pSprite->position.x - pSprite->speed.x;
			if (x < -TREE_WIDTH)
				x = WIDTH - pSprite->position.x;
			SetSpritePosition(pSprite, x, pSprite->position.y);
				
			break;
	}
}

// Replaces the sprite with a potion of the background
void EraseSprite(BUFFER *pBuffer, SPRITE *pSprite, HBITMAP hBackground)
{
	// Store the portion of the background that we need to redraw
	// We add and subtract the speed because if we switch states it sometimes moves
	// Without erasing the current position.
	RECT Portion= {pSprite->position.x - pSprite->speed.x, 
				   pSprite->position.y - pSprite->speed.y, 
				   pSprite->position.x + pSprite->width + pSprite->speed.x, 
				   pSprite->position.y + pSprite->height + pSprite->speed.y};

	// Display only the portion of the background that needs to be redrawn
	DisplayPortionOfBitmap(pBuffer, hBackground, pSprite->position.x - pSprite->speed.x, pSprite->position.y - pSprite->speed.y, Portion);
}

// Frees the sprite and it's associated hBitmaps 
void FreeSprite(SPRITE *pSprite)
{
	// Check if the handle to the bitmap is valid
	if(pSprite->hBitmap)
	{
		// Free the hBitmap
		DeleteObject(pSprite->hBitmap);			
	}
}	