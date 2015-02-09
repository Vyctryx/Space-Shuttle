#ifndef _BITMAP_H				
#define _BITMAP_H				 

#include "main.h"
						
#define TRANSPARENT_COLOR RGB(0, 0, 0)

struct BUFFER					// Back buffering structure
{
	HWND hwnd;					// Current window's handle
	RECT scrnRect;				// Client rectangle of the window
	HANDLE hCompBitmap;			// Compatible bitmap for the backbuffer
	HANDLE hOldBitmap;			// Used for storage to free when the program quits
	HDC hdcFront;				// Front buffer
	HDC hdcBack;				// Back buffer
	HDC hdcBitmap;				// Temp buffer
};

// Creates double buffering
void CreateDoubleBuffering(BUFFER *pBuffer, HWND hwnd);
// Loads a bitmap
HBITMAP LoadABitmap(LPSTR szFileName);
// Displays the bitmap to the screen at x,y
void DisplayBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y);
// Displays a portion of a bitmap to the back buffer
void DisplayPortionOfBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion);
// Displays a portion of a bitmap with a transparency to (0, 0, 0)
void DisplayTransparentBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion);
// Stretches the image to the window rectangle to fill up the background
void DrawBackgroundImage(BUFFER *pBuffer, HBITMAP hBitmap);
// Clears the backbuffer to what ever colour is passed in
void ClearScreen(HDC hdc, RECT scrnRect, int color);
// Swaps the back buffer to the screen
void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer);
// Resizes the buffers to the current window size
void ResizeBuffers(BUFFER *pBuffer);
// Frees the back buffer information
void FreeBuffers(BUFFER *pBuffer);
// This rotates a bitmap - does not work
//HBITMAP RotateBitmap( HBITMAP hBitmap, float radians, HDC hDC, int srcX, int srcY);

#endif

