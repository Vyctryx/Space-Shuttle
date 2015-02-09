#include "main.h"

// Sets up the double buffering by filling in the 
// buffer structure that is passed in.
void CreateDoubleBuffering(BUFFER *pBuffer, HWND hwnd)
{
	// Assign the window handle to our structure
	pBuffer->hwnd = hwnd;									
	
	// Store the client rectangle in our structure
	GetClientRect(hwnd, &(pBuffer->scrnRect));

	// Get a handle to the device context and store it as the front buffer
	pBuffer->hdcFront = GetDC(pBuffer->hwnd);

	// Create a compatible device context for the backbuffer
	pBuffer->hdcBack = CreateCompatibleDC(pBuffer->hdcFront);

	// Create another hdc to store the bitmap in before the backbuffer
	pBuffer->hdcBitmap = CreateCompatibleDC(pBuffer->hdcFront);

	// Create a dummy bitmap handle to store in the front buffer
	pBuffer->hCompBitmap = CreateCompatibleBitmap(pBuffer->hdcFront, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom);

	// Select the dummy bitmap handle into the back buffer to make it like the front hdc
	pBuffer->hOldBitmap = (HBITMAP)SelectObject(pBuffer->hdcBack, pBuffer->hCompBitmap);	
}

//Loads a bitmap and returns a handle to that bitmap.
HBITMAP LoadABitmap(LPSTR szFileName)
{
	// Load the bitmap and return the handle to the bitmap we just loaded
	return (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

//Puts the bitmap in the backbuffer
void DisplayBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y)
{
	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer
	BitBlt(pBuffer->hdcBack, x, y, pBuffer->scrnRect.right, 
		   pBuffer->scrnRect.bottom,  pBuffer->hdcBitmap, 0, 0, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

// Takes a portion of a bitmap and puts it in the backbuffer
void DisplayPortionOfBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion)
{
	// Assign the width and height to shorten variables. 
	int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	BOOL result = BitBlt(pBuffer->hdcBack, x, y, width, height, 
				   pBuffer->hdcBitmap, rPortion.left, rPortion.top, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

// Blits a bitmap hdc into the backbuffer using the transparent colour 0
void TransparentBlit(HDC hdc, int destX, int destY, int destWidth, int destHeight, 
					 HDC hdc2, int srcX, int srcY, UINT tranparency)
{
	unsigned char* pImageBits;
	unsigned char* pBackBits;
	BITMAPINFO bmBitmapInfo = {0};
	HBITMAP hBitmap, hBitmap2, hOldBitmap, hOldBitmap2;
	HDC compHDC;
	HDC compHDC2;
	
	// Fill in our BitmapInfo structure (we want a 24 bit image)
	bmBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmBitmapInfo.bmiHeader.biCompression = BI_RGB;
	bmBitmapInfo.bmiHeader.biHeight = destHeight;
	bmBitmapInfo.bmiHeader.biWidth = destWidth;	
	bmBitmapInfo.bmiHeader.biBitCount = 24;
	bmBitmapInfo.bmiHeader.biClrUsed = 0;
	bmBitmapInfo.bmiHeader.biPlanes = 1;

	// Create 2 DIB Sections.  One for the Front Buffer and one for the BackBuffer
	hBitmap  = CreateDIBSection(hdc,  &bmBitmapInfo, DIB_RGB_COLORS,(void **)&pImageBits, 0,0);	
	hBitmap2  = CreateDIBSection(hdc,  &bmBitmapInfo, DIB_RGB_COLORS,(void **)&pBackBits, 0,0);

	// Create a compatible DC for the front buffer and Select our Dib Section into it
	compHDC = CreateCompatibleDC(hdc);
	hOldBitmap = (HBITMAP)SelectObject(compHDC, hBitmap);
	
	// Create a compatible DC for the back buffer and Select our Dib Section into it
	compHDC2 = CreateCompatibleDC(hdc2);
	hOldBitmap2 = (HBITMAP)SelectObject(compHDC2, hBitmap2);

	// Blit the front buffer to our compatible DC that will hold the destination bits
	BitBlt(compHDC, 0, 0, destWidth, destHeight, hdc, destX, destY, SRCCOPY);

	// Blit the back buffer to our compatible DC that will hold the source bits
	BitBlt(compHDC2, 0, 0, destWidth, destHeight, hdc2, srcX, srcY, SRCCOPY);

	// Loop through the 24 bit image (Times 3 for R G and B)
	for(int i = 0; i < destHeight * destWidth * 3; i += 3)
	{
		// Check if the current pixel being examined isn't the transparent color
		if((pBackBits[i]   != GetBValue(tranparency)) || 
		   (pBackBits[i+1] != GetGValue(tranparency)) || 
		   (pBackBits[i+2] != GetRValue(tranparency)))
		{
			// Assign the desired pixel to the foreground
			pImageBits[i]     = pBackBits[i];
			pImageBits[i + 1] = pBackBits[i + 1];
			pImageBits[i + 2] = pBackBits[i + 2];
		}
	}  
	
	// Blit the transparent image to the front buffer (Voila!)
	BitBlt(hdc, destX, destY, destWidth, destHeight, compHDC, 0, 0, SRCCOPY);

	// Cleanup the monochrome bitmaps
	SelectObject(compHDC, hOldBitmap);
	SelectObject(compHDC2, hOldBitmap2);	

	// Free GDI Resources
	DeleteObject(hBitmap);
	DeleteObject(hBitmap2);
	DeleteDC(compHDC);
	DeleteDC(compHDC2);
}

// Takes a portion of a bitmap and puts it in the backbuffer
// but, not displaying any color with rgb values of (0,0,0).
void DisplayTransparentBitmap(BUFFER *pBuffer, HBITMAP hBitmap, int x, int y, RECT rPortion)
{
	// Assign the width and height to shorten variables. 
	int width = rPortion.right - rPortion.left, height = rPortion.bottom - rPortion.top;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	TransparentBlit(pBuffer->hdcBack, x, y, width, height, 
				    pBuffer->hdcBitmap, rPortion.left, rPortion.top, TRANSPARENT_COLOR);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

// Draws and if desired, stretches an image to the backbuffer
void DrawBackgroundImage(BUFFER *pBuffer, HBITMAP hBitmap)
{
	// This will hold our information on the bitmap
	BITMAP bitmapInfo;

	// Select the bitmap handle into the extra hdc that holds the bitmap
	HBITMAP hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBitmap, hBitmap);

	// Get the width and height of the background bitmap
	GetObject(hBitmap, sizeof(BITMAP), &bitmapInfo);

	// Blit the bitmap hdc into the backbuffer using the transparent color 0 (black)
	BOOL result = StretchBlt(pBuffer->hdcBack, 0, 0, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom, 
				   pBuffer->hdcBitmap, 0, 0, bitmapInfo.bmWidth, bitmapInfo.bmHeight, SRCCOPY);

	// Select the old handle to the bitmap back where it was to clean up
	SelectObject(pBuffer->hdcBitmap, hOldBitmap);
}

// Fills the rectangle passed in with a specified color
void ClearScreen(HDC hdc, RECT scrnRect, int color)
{
	// Fill the backbuffer with white to erase the previous position of the bitmap
	FillRect(hdc, &scrnRect, (HBRUSH)GetStockObject(color));
}

// Flips the backbuffer to the front of the screen,
// which displays the bitmaps to the screen.
void SwapBackBuffer(BUFFER *pBuffer, BOOL bClearBackBuffer)
{
	// Blit the whole backbuffer to the screen (The front buffer)
	BitBlt(pBuffer->hdcFront, pBuffer->scrnRect.left, pBuffer->scrnRect.top, 
		   pBuffer->scrnRect.right, pBuffer->scrnRect.bottom, pBuffer->hdcBack, 0, 0, SRCCOPY);

	// If we want to clear the screen afterwards, set the background to white
	if(bClearBackBuffer) ClearScreen(pBuffer->hdcBack, pBuffer->scrnRect, WHITE_BRUSH);
}

// Changes the size of the front and back buffers according 
// to the new client size of the window.
void ResizeBuffers(BUFFER *pBuffer)
{
	// Get the new client rectangle of the window
	GetClientRect(pBuffer->hwnd, &(pBuffer->scrnRect));
			
	// Select the old bitmap back into the back buffer
	SelectObject(pBuffer->hdcBack, pBuffer->hOldBitmap);

	// Delete the old compatible bitmap
	DeleteObject(pBuffer->hCompBitmap);

	// Create a new compatible bitmap with the new size of the window
	pBuffer->hCompBitmap = CreateCompatibleBitmap(pBuffer->hdcFront, pBuffer->scrnRect.right, pBuffer->scrnRect.bottom);

	// Select the new compatible bitmap into the backbuffer and store the old one again
	pBuffer->hOldBitmap = (HBITMAP) SelectObject(pBuffer->hdcBack, pBuffer->hCompBitmap);	
}

//Frees the buffer for the window
void FreeBuffers(BUFFER *pBuffer)
{
	// Set the back buffer to it's original bitmap (This is necessary for clean-up)
	SelectObject(pBuffer->hdcBack, pBuffer->hOldBitmap);

	// Free the hbitmaps
	DeleteObject(pBuffer->hOldBitmap);
	DeleteObject(pBuffer->hCompBitmap);

	// Free all the HDC's
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcBack);
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcBitmap);
	ReleaseDC(pBuffer->hwnd, pBuffer->hdcFront);
}

// Creates a new bitmap with rotated image
HBITMAP RotateBitmap( HBITMAP hBitmap, float radians, HDC hDC, int srcX, int srcY) //int& destX, int& destY )
{
	// Create a memory DC compatible with the display
	HDC sourceDC, destDC;
	sourceDC = CreateCompatibleDC( NULL );
	destDC = CreateCompatibleDC( NULL );
	float x1, x2, x3, x4, y1, y2, y3, y4, cA, sA, CtX, CtY;
	int ofX, ofY;
	int destX, destY;
	// Get logical coordinates
	BITMAP bm;
	GetObject( hBitmap, sizeof( bm ), &bm );


	CtX = ((float) srcX) / 2;
	CtY = ((float) srcY) / 2;

	cA = (float) cos (radians);
	sA = (float) sin (radians);

	x1 = CtX + (-CtX) * cA - (-CtY) * sA;
	x2 = CtX + (srcX - CtX) * cA - (-CtY) * sA;
	x3 = CtX + (srcX - CtX) * cA - (srcY - CtY) * sA;
	x4 = CtX + (-CtX) * cA - (srcY - CtY) * sA;

	y1 = CtY + (-CtY) * cA + (-CtX) * sA;
	y2 = CtY + (srcY - CtY) * cA + (-CtX) * sA;
	y3 = CtY + (srcY - CtY) * cA + (srcX - CtX) * sA;
	y4 = CtY + (-CtY) * cA + (srcX - CtX) * sA;

    ofX = min(0,min(x1, min(x2,x3)));
	ofY = min(0,min(y1, min(y2,y3)));
	destX = max(0,max(x1, max(x2,x3)));
	destY = max(0,max(y1, max(y2,y3)));

	// Create a bitmap to hold the result
	HBITMAP hbmResult = CreateCompatibleBitmap(hDC, destX, destY);//w, h);

	HBITMAP hbmOldSource = (HBITMAP)SelectObject( sourceDC, hBitmap );
	HBITMAP hbmOldDest = (HBITMAP)SelectObject( destDC, hbmResult );

	// Draw the background color before we change mapping mode
	HBRUSH hbrBack = CreateSolidBrush(BLACK_BRUSH);
	HBRUSH hbrOld = (HBRUSH)SelectObject(destDC, hbrBack);
	PatBlt(destDC, 150, 150, 72, 77, PATCOPY);
	DeleteObject(SelectObject(destDC, hbrOld));

	// We will use world transform to rotate the bitmap
	SetGraphicsMode(destDC, GM_ADVANCED);
	XFORM xform;
	xform.eM11 = cA;
	xform.eM12 = -sA;
	xform.eM21 = sA;
	xform.eM22 = cA;
	xform.eDx = 0;
	xform.eDy = 0;

	SetWorldTransform( destDC, &xform );

	// Now do the actual rotating
	BitBlt(destDC,0,0,bm.bmWidth, bm.bmHeight, sourceDC, 0, 0, SRCCOPY );

	// Restore DCs
	SelectObject( sourceDC, hbmOldSource );
	SelectObject( destDC, hbmOldDest );

	return hbmResult;
}
