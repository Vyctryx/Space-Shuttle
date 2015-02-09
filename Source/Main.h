#ifndef _MAIN_H									
#define _MAIN_H				

#include <iostream>			
#include <windows.h>		
#include <mmsystem.h>		
#include <math.h>
#include <stdio.h>
#include "bitmap.h"			
#include "sprite.h"			
#include "scores.h"
using namespace std;																	

#define HEIGHT         480			// Window height
#define WIDTH          640			// Window width
#define FRAME_RATE     60			// Frames per second
				
// Function prototype for our WndProc																																																																									
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);		
// Loads the bitmaps and creates the double buffer
void Init(HWND hwnd);
// Frees memory and quits the program
void DeInit();
// Prints game information to screen
void PrintInfo(HWND hwnd, BUFFER *pBuffer, int distance, int speed, int fuel,
			   int angle, int time, bool gameOverFlag, bool landedFlag, int score);
// Changes to fullscreen or windowed mode
bool ChangeDisplayMode(int pixel_width, int pixel_height, bool switch_back = false);

#endif

