#include <windows.h>
#include <iostream>
#include "main.h"											
#include "resource.h"
#include <stdlib.h>
using namespace std;

// Width and heights of sprite bitmaps
#define SHUTTLE_WIDTH  134
#define SHUTTLE_HEIGHT 55
#define TREE_WIDTH 99
#define TREE_HEIGHT 74
#define TREE_WIDTH2 129
#define TREE_HEIGHT2 119
#define CLOUD_WIDTH 129
#define CLOUD_HEIGHT 119
#define OVER_WIDTH 343
#define OVER_HEIGHT 259
#define JUNK_WID 122
#define JUNK_HGT 99
#define MOON_WID 46
#define MOON_HGT 45

// Amount of frames we animate for the shuttle in one cycle of animation
#define SHUTTLE_FRAMES  1
#define JUNK_FRAMES 4

// Create our double buffering structure
BUFFER gBuffer;	
	
// Game variables
int totalDistance;
float speedx;
float speedy;
float treeSpeed;
float cloudSpeed;
float moonSpeed;
float junkSpeed;
int dist;
int time;
int angleOfAttack;
int fuel;
int counter = 0;
int score;
float frameInterval;
bool stallFlag;	
bool userControl;									
bool fullscreen;
bool takeOff;
bool crash;
bool gameOverFlag;
bool landed;
bool nextScene;
bool calcScore;
bool renamed;
bool pause;
bool dispScore;
int distRemain;
int finalScore =0;
string userName;
float degrees = 0;
double pi = 3.142;
double radians = (2*pi *degrees)/360;	
// Create sprites
SPRITE shuttle;
SPRITE tree;
SPRITE tree2;
SPRITE cloud;
SPRITE explode;
SPRITE crashBmp;
SPRITE gameOver;
SPRITE junk;
SPRITE stall;
SPRITE dive;
SPRITE land;
SPRITE moon;

// Holds the background bitmap
HBITMAP hBackground;
HINSTANCE hInt; 
				

LRESULT CALLBACK DlgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BUFFER *pBuffer)
{			
	char userName[25]={0};				
    switch( message )									
    {
		case WM_INITDIALOG:								
            return TRUE;

		case WM_COMMAND:								
   			switch( LOWORD( wParam ) )					
            {    
				case IDOK:																		
					GetDlgItemText(hWnd, IDC_EDIT1, userName, 25);
					EndDialog( hWnd, TRUE );					 
            }
            break;

        case WM_CLOSE:									
			EndDialog( hWnd, FALSE );					 
			break;

		case WM_DESTROY:								
        	break;											
    }
    return FALSE;											
}
																										
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)			
{		
	hInt = hInstance;
	// Create the handle to the window.  
    HWND        hwnd;										
	// Create the variable to hold the window messages
    MSG         msg;	

    WNDCLASSEX  wndclass = {0};
	
	wndclass.cbSize = sizeof (wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW; 
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon (NULL, IDI_WINLOGO);
	wndclass.hCursor = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszClassName = "Window Class";
	wndclass.lpszMenuName = "thing";
	wndclass.hIcon = LoadIcon (NULL, IDI_WINLOGO);

	// Register the window class with the operating system
	RegisterClassEx (&wndclass);	
	
	// Create the window
    hwnd = CreateWindow ("Window Class",					
						 "Space Shuttle Game",	  			
						 WS_OVERLAPPED | WS_SYSMENU,
						 CW_USEDEFAULT,						
						 CW_USEDEFAULT,						
						 WIDTH,									 
						 HEIGHT,						    
						 NULL,								
						 NULL,								
						 hInstance,						    
						 NULL);								

	// Show window. 
    ShowWindow (hwnd, iCmdShow);							

	// Paint our window to screen.
    UpdateWindow (hwnd);								

	// Main loop. 
	while (1)					
    {	
		// Check if there is a window message and remove it from the queue
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// If the message was to quit, break out of the while loop
			if(msg.message == WM_QUIT) break;

			// Translates message
			TranslateMessage (&msg);					
		
			// This sends the messages to WndProc().
			DispatchMessage (&msg);							
		}
		else
		{
			// Check if we want to animate the next frame of animation
			if(AnimateNextFrame(FRAME_RATE))
			{
				if(!pause) // If the player has not paused the game
				{
					int fuelUse;
					
				
					

					// If the game is not over yet
					if (!gameOverFlag && !landed)
					{
						// Physics for calculating distance travelled
						time++;
						dist = dist + speedx * time;
						distRemain = totalDistance - dist;
						// increment the score for each frame
						score++;				
					}
				
					// If the game is over
					if(gameOverFlag || landed)
					{
						// Set speeds to 0
						SetSpriteSpeed(&shuttle, 0, 0);
						SetSpriteSpeed(&explode, 0, 0);
					}
				
				
					// Calculate rate of fuel use
					if (speedx > 15)
					{
						fuelUse = speedx * 3;
					}
					else if (speedx > 10)
					{
						fuelUse = speedx * 2;
					}
					else if (speedx == 0)
					{
						fuelUse = 0;
					}
					else
					{
						fuelUse = speedx;
					}
					// Calculate remaining fuel
					fuel = fuel - fuelUse;

					// Check for a stall. Usually if a 16 degree angle of attack is exceeded, 
					// the shuttle would stall - I have scaled this down for the game.
					if (angleOfAttack > 8)
					{
						// Set stall to true and take away player control
						stallFlag = true;
						userControl = false;
						// Set the sprite speeds
						treeSpeed = treeSpeed - 3;
						cloudSpeed = cloudSpeed - 1;
						junkSpeed = junkSpeed - 2;
						moonSpeed--;
						speedx = speedx - 2;
						// Animate the stall sprite
						AnimateSprite(&gBuffer, &stall, radians, angleOfAttack);
						// Set speeds and states
						SetSpriteSpeed(&tree, treeSpeed, 0);
						SetSpriteSpeed(&tree2, speedx, 0);
						SetSpriteSpeed(&cloud, cloudSpeed, 0);
						SetSpriteSpeed(&junk, junkSpeed, 0);
						SetSpriteSpeed(&moon, moonSpeed, 0);
						SetSpriteSpeed(&shuttle, speedx, speedy);
						SetSpriteState(&shuttle, SOUTH);
					}
					else if(angleOfAttack < -8) 
					{
						// Set stall flag to true and take away user control
						stallFlag = true;
						userControl = false;
						// Set sprite speeds
						treeSpeed = treeSpeed - 3;
						cloudSpeed = cloudSpeed - 1;
						junkSpeed = junkSpeed - 1;
						speedx = speedx - 2;
						moonSpeed--;
						// Animate dive sprite
						AnimateSprite(&gBuffer, &dive, radians, angleOfAttack);
						// Set speeds
						SetSpriteSpeed(&tree, 0, 0);
						SetSpriteSpeed(&tree2, 0, 0);
						SetSpriteSpeed(&cloud, 0, 0);
						SetSpriteSpeed(&moon, 0, 0);
						SetSpriteSpeed(&junk, 0, 0);
						SetSpriteSpeed(&shuttle, speedx, speedy);
						SetSpriteState(&shuttle, SOUTH);
					}
					
					// Check that we still have fuel
					if (fuel <= 0)
					{
						// Set fuel to zero and stall to true
						fuel = 0;
						stallFlag = true;
						// Set shuttle speed and state
						SetSpriteSpeed(&shuttle, speedx, 10);
						SetSpriteState(&shuttle, SOUTH);
					}

					// Check which scene we're in
					// If we're in the earth scene and we go too high
					if (shuttle.position.y <= 10 && nextScene)
					{
						// Set the scene to the space scene
						nextScene = false;
						hBackground = LoadABitmap("background.bmp");
					    ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			
						DisplayBitmap(&gBuffer, hBackground, 0, 0);
						// Set shuttles new y position
						shuttle.position.y = 348;
					}
					// If we're in the space scene and we go too high
					else if(shuttle.position.y <= 10 && !nextScene)
					{
						// Crash the shuttle
						crash = true;
						speedx = 0;
						speedy = 0;
						SetSpriteSpeed(&shuttle, 0, 0);
						SetSpriteSpeed(&explode, 0, 0);
					}
					// If we're in the earth scene and we go too low
					else if(shuttle.position.y >= 350 && nextScene)
					{
						// If we are close enough to land, and our speed
						// and angle are within an acceptable range
						if((angleOfAttack >= -2 && angleOfAttack <= 2) && 
							(speedx >= 4 && speedx <= 9) && 
							((dist / 100) > 25000 && (dist / 100) < 31000))
						{
							// We've landed
							landed = true;
				//			score = score + 500;	
				//			finalScore = score + ((totalDistance - fuel) / time);
						}
						else
						{
							// We've crashed
							speedx = 0;
							speedy = 0;
							SetSpriteSpeed(&shuttle, speedx, 0);
							SetSpriteSpeed(&tree, speedx, 0);
							SetSpriteSpeed(&tree2, speedx, 0);
							SetSpriteSpeed(&cloud, speedx, 0);
							
							crash = true;
						}
					}
					// If we're in the space scene and we go too low
					else if(shuttle.position.y >= 350 && !nextScene)
					{
						// Load the earth scene
						nextScene = true;
						shuttle.position.y = 11;
						hBackground = LoadABitmap("Sky.bmp");
					    ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			
						DisplayBitmap(&gBuffer, hBackground, 0, 0);

					}

					// If we slow down too much mid-air
					if (speedx < 4 && !landed)
					{
						if (!gameOverFlag)
						{
							// We've stalled
							stallFlag = true;
							// Set speeds and animate stall sprite
							SetSpriteSpeed(&tree, 0, 0);
							SetSpriteSpeed(&tree, 0, 0);
							SetSpriteSpeed(&cloud, 0, 0);
							SetSpriteSpeed(&junk, 0, 0);
							SetSpriteSpeed(&moon, 0, 0);
							SetSpriteSpeed(&shuttle, speedx, 10);
							SetSpriteState(&shuttle, SOUTH);
							AnimateSprite(&gBuffer, &stall, radians, angleOfAttack);
						}
					}

					// If we go too fast for the shuttle to handle
					if (speedx > 30)
					{
						if (!gameOverFlag)
						{
							// Shuttle explodes
							crash = true;
							speedx = 0;
							speedy = 0;
							SetSpriteSpeed(&shuttle, speedx, speedy);
						}
					}

					// Check that we have not exceeded the total distance
					if (distRemain <= 0)
					{
						// Gone too far without landing - Game Over.
						gameOverFlag = true;
						SetSpriteSpeed(&shuttle, speedx, 0);
						speedx = 0;
					}


					// Check if the game is over
					if(gameOverFlag)
					{
						// If we've crashed
						if(crash)
						{
							speedx = 0;
							EraseSprite(&gBuffer, &dive, hBackground);
							EraseSprite(&gBuffer, &stall, hBackground);
							SetSpritePosition(&explode, shuttle.position.x, shuttle.position.y - 40);
							SetSpriteSpeed(&explode, 0, 0);
							// Animate our crash and game over sprites
							AnimateSprite(&gBuffer, &explode, radians, angleOfAttack); 			
							AnimateSprite(&gBuffer, &crashBmp, radians, angleOfAttack); 
							AnimateSprite(&gBuffer, &gameOver, radians, angleOfAttack); 
						}						
						else
						{
							// Just animate our game over sprite
							AnimateSprite(&gBuffer, &gameOver, radians, angleOfAttack); 
						}
					}
					else
					{
						// If we're in the earth scene
						if (nextScene)
						{
							// Animate the earth scene sprites
							AnimateSprite(&gBuffer, &tree2, 0, angleOfAttack);	
							AnimateSprite(&gBuffer, &cloud, 0, angleOfAttack);
						}
						else
						{
							// Animate space scene sprites
							AnimateSprite(&gBuffer, &junk, 0, angleOfAttack);
							AnimateSprite(&gBuffer, &moon, 0, angleOfAttack);
						}
						// If we've landed
						if(landed)
						{
							speedx = 0;
							angleOfAttack = 0;
							SetSpriteSpeed(&shuttle, speedx, 0);
							crash = false;
							stallFlag = false;
							if(!calcScore || counter == 0)
							{
							//	score = score + 500;
								score = score + ((totalDistance - fuel) / time);
								calcScore = true;
								counter++;
							}
							AnimateSprite(&gBuffer, &shuttle, radians, angleOfAttack); 
							AnimateSprite(&gBuffer, &land, radians, angleOfAttack); 
		
						}
						// If we've crashed
						else if (crash)
						{
							gameOverFlag = true;
							speedx = 0;
							speedy = 0;
							if(!gameOverFlag)
							{
								score = score + (fuel / 10);
							}
							SetSpritePosition(&explode, shuttle.position.x,
								shuttle.position.y - 40);
							SetSpriteSpeed(&explode, 0, 0);
						}
						else
						{
							AnimateSprite(&gBuffer, &shuttle, radians, angleOfAttack); 
							if (nextScene)
							{
								AnimateSprite(&gBuffer, &tree, 0, angleOfAttack);	
							}
						}
					}

					if (counter > 0 && counter < 2)
					{ 
						score = (score / 2);
						counter ++;
					}
				
					// If game is over
					if (landed || gameOverFlag)
					{	
						finalScore = score + finalScore;
						score = finalScore;
						finalScore = 0;
						if (dispScore)
						{
							// If player has achieved a high score
							if(CalculateList(score, &gBuffer, renamed))//CalculateList(score, &gBuffer, renamed))
							{
								DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1),
									NULL, (DLGPROC)DlgProc );
								outputScores(hwnd);
								AnimateSprite(&gBuffer, &land, radians, angleOfAttack); 
								dispScore = false;
							}
							calcScore = true;
						}
					}
					PrintInfo(hwnd, &gBuffer, distRemain, speedx, fuel, angleOfAttack, time,
					gameOverFlag, landed, score);
					// Move the sprites along the screen
					MoveSprite(&tree);
					MoveSprite(&tree2);
					MoveSprite(&cloud);
					MoveSprite(&shuttle);
					MoveSprite(&junk);
					MoveSprite(&moon);
				}
				else
				{
					// Print our game info
					PrintInfo(hwnd, &gBuffer, distRemain, speedx, fuel, angleOfAttack, time,
						gameOverFlag, landed, score);
				}
				// Swap the backbuffer to display it to the front and clear the screen
				SwapBackBuffer(&gBuffer, FALSE);

				// Erase the position of the shuttle with a part of the background
				if(!pause) // As long as the game is not paused
				{
					EraseSprite(&gBuffer, &shuttle, hBackground);
					EraseSprite(&gBuffer, &tree, hBackground);
					EraseSprite(&gBuffer, &tree2, hBackground);
					EraseSprite(&gBuffer, &cloud, hBackground);
					EraseSprite(&gBuffer, &junk, hBackground);
					EraseSprite(&gBuffer, &moon, hBackground);
					EraseSprite(&gBuffer, &explode, hBackground);

					stallFlag = false;
				}
			}
		}
	}

	// Unregister the window class with the operating system
	UnregisterClass("Window Class",hInstance);
    
	// Quit the program
	return msg.wParam ;										
}
												
LRESULT CALLBACK WndProc (HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
		
	switch(iMsg)
    {
	
		case WM_COMMAND:
			if(LOWORD(wParam) == IDM_QUIT)
			{
				PostQuitMessage(0);
			}
			else if(LOWORD(wParam) == ID_OPTIONS_NEWGAME)
			{
				Init(hwnd);
			}
			else if(LOWORD(wParam) == ID_FULLSCREEN_ON)
			{
				if(fullscreen == false)
				{
					// Try and change to full screen mode
					if(!ChangeDisplayMode(WIDTH,HEIGHT))
					{
						MessageBox(hwnd,"Couldn't change to full screen!","Warning",MB_OK);
						return 0;
					}
					// Otherwise we made it 
					fullscreen = true;
					MoveWindow(hwnd,0,0,WIDTH,HEIGHT,false);
				}
			}
			else if(LOWORD(wParam) == ID_FULLSCREEN_OFF)
			{
				if(fullscreen == true)
				{
					ChangeDisplayMode(0,0,true);
					fullscreen = false; // No longer in full screen mode
					ShowCursor(TRUE);
				}
			}
			else if(LOWORD(wParam) == ID_OPTIONS_HIGHSCORES)
			{
				outputScores(hwnd);
			}
			else if(LOWORD(wParam) == ID_HELP_CONTROLS)
			{
				char help1[512] = {0};
				char help2[512] = {0};
				char help3[512] = {0};
				char help4[512] = {0};
				char help5[512] = {0};
				char help6[512] = {0};
				char help7[512] = {0};
				char help8[512] = {0};
				char help9[512] = {0};
				char help10[512] = {0};
				char final[512] = {0};
				sprintf(help1, "Left - shuttle speeds up \n");
				sprintf(help2, "Right - shuttle slows down \n");
				sprintf(help3, "Up - shuttle's height above ground increases\n");
				sprintf(help4, "Down - shuttle's height above ground decreases\n");
				sprintf(help5, "F1 - Toggle fullscreen mode\n");
				sprintf(help6, "F2 - Toggle window mode\n");
				sprintf(help7, "F3 - Plane stalls and crashes\n");
				sprintf(help8, "F4 - Restart game\n");
				sprintf(help9, "F5 - Pause\n");
				sprintf(help10, "ESC - Quit game");
				strncat(final, help1, 512);
				strncat(final, help2, 512);
				strncat(final, help3, 512);
				strncat(final, help4, 512);
				strncat(final, help5, 512);
				strncat(final, help6, 512);
				strncat(final, help7, 512);
				strncat(final, help8, 512);
				strncat(final, help9, 512);
				strncat(final, help10, 512);
				MessageBox(hwnd, final, "Player Controls", MB_OK);
			}
			else if(LOWORD(wParam) == ID_HELP_ABOUT)
			{
				char about1[512] = {0};
				char about2[512] = {0};
				sprintf(about1, "This game was created by Lauren Marsh on\n");
				sprintf(about2, "10th May 2003, last modified on 20th May 2003.\n");
				strncat(about1, about2, 512);
				MessageBox(hwnd, about1, "About", MB_OK);
			}
			else if(LOWORD(wParam) == ID_HELP_RULES)
			{
				char rules[512] = {0};
				char rules2[512] = {0};
				char rules3[512] = {0};
				char rules4[512] = {0};
				char rules5[512] = {0};
				char rules6[512] = {0};
				char rules7[512] = {0};
				char rules8[512] = {0};
				char rules9[512] = {0};
				char rules10[512] = {0};
				char rules11[512] = {0};
				char rules12[512] = {0};
				char rules13[512] = {0};
				sprintf(rules, "The aim of the game is to land the space\n");
				sprintf(rules2, "shuttle before you pass the runway, and within\n");
				sprintf(rules3, "a reasonable range of angles and speeds.\n");
				sprintf(rules4, "If the shuttle flies too low, it will crash,\n");
				sprintf(rules5, "and if it flies too high it will burn up in the\n");
				sprintf(rules6, "atmosphere. The shuttle will stall if it climbs at\n");
				sprintf(rules7, "an angle of attack greater than 16 degrees, and\n");
				sprintf(rules8, "will go into an uncontrollable dive if it descends\n");
				sprintf(rules9, "too steeply. It will also explode it is flown too fast.\n");
				sprintf(rules10, "To land the shuttle, you need to be within 5000 feet\n");
				sprintf(rules11, "of the runway, have an angle of attacl between -4\n");
				sprintf(rules12, "and 4 degrees, and have an airspeed between 20 and\n");
				sprintf(rules13, "45 knots.");
				strncat(rules, rules2, 512);
				strncat(rules, rules3, 512);
				strncat(rules, rules4, 512);
				strncat(rules, rules5, 512);
				strncat(rules, rules6, 512);
				strncat(rules, rules7, 512);
				strncat(rules, rules8, 512);
				strncat(rules, rules9, 512);
				strncat(rules, rules10, 512);
				strncat(rules, rules11, 512);
				strncat(rules, rules12, 512);
				strncat(rules, rules13, 512);
				MessageBox(hwnd, rules, "Game Rules", MB_OK);
			}
			break;
			
		case WM_PAINT:
		    BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			return 0;
	
		case WM_CREATE:

			// Create the double buffer and load the bitmaps	
			Init(hwnd);										
			break;	

		case WM_DESTROY:									
			if(fullscreen)
				ChangeDisplayMode(0,0,true);
			// Free all the data and set everything back to normal, then post the quit message
			DeInit();										
			break;		
		
		case WM_KEYDOWN:
			switch (wParam) 
			{
				case VK_LEFT:
					if(userControl) // If we haven't stalled or dived
					{
						if(speedx > 0)
						{
							speedx = speedx - 2;
							treeSpeed = treeSpeed - 3;
							cloudSpeed = cloudSpeed--;
							moonSpeed--;
							junkSpeed = junkSpeed- 2;

							// Set sprite speeds and states
							SetSpriteSpeed(&tree, treeSpeed, 0);
							SetSpriteSpeed(&tree2, speedx, 0);
							SetSpriteSpeed(&cloud, cloudSpeed, 0);
							SetSpriteSpeed(&junk, junkSpeed, 0);
							SetSpriteSpeed(&moon, moonSpeed, 0);
							SetSpriteSpeed(&shuttle, speedx, speedy);
						}
					}
					break;

				case VK_RIGHT:
					if(userControl) // If we haven't stalled or dived
					{
						if (!crash)
						{
							// Increase or decrease our angle of attack
							if(angleOfAttack < 0)
								angleOfAttack++;
							else if(angleOfAttack > 0)
								angleOfAttack--;

							// Set speeds and states
							SetSpriteState(&shuttle, EAST);

							speedx = speedx + 2;
							treeSpeed = treeSpeed + 3;
							cloudSpeed = cloudSpeed++;
							junkSpeed = junkSpeed + 2;
							moonSpeed++;

							SetSpriteSpeed(&tree, treeSpeed, 0);
							SetSpriteSpeed(&moon, moonSpeed, 0);
							SetSpriteState(&tree, WEST);
							SetSpriteSpeed(&tree2, speedx, 0);
							SetSpriteSpeed(&cloud, cloudSpeed, 0);
							SetSpriteState(&cloud, WEST);
							SetSpriteSpeed(&junk, junkSpeed, 0);
							SetSpriteSpeed(&shuttle, speedx, speedy);
						}
					}
					break;

				case VK_UP:
					if(userControl)
					{
						if (!crash)
						{
							angleOfAttack++;
							degrees++;
							radians = (2*pi *degrees)/360;
							speedy = speedx;
							// Set shuttle speed and state
							SetSpriteSpeed(&shuttle, speedx, speedy);
							SetSpriteState(&shuttle, NORTH);
						}
					}
					break;

				case VK_DOWN:
					if(userControl)
					{
						if (!crash)
						{
							angleOfAttack--;
							degrees--;
							radians = (2*pi *degrees)/360;
							speedy = speedx;
							SetSpriteSpeed(&shuttle, speedx, speedy);
							SetSpriteState(&shuttle, SOUTH);
						}
					}
					break;

				case VK_F3:
					speedx = 0;
					SetSpriteSpeed(&shuttle, 0, 0);
					SetSpriteSpeed(&tree, 0, 0);
					SetSpriteSpeed(&tree2, 0, 0);
					SetSpriteSpeed(&cloud, 0, 0);
					break;
				case VK_F1:

					if(fullscreen == false)
					{
						// Try and change to full screen mode
						if(!ChangeDisplayMode(WIDTH,HEIGHT))
						{
							MessageBox(hwnd,"Couldn't change to full screen!","Warning",MB_OK);
							return 0;
						}
						// Otherwise we made it
						fullscreen = true;
						MoveWindow(hwnd,0,0,WIDTH,HEIGHT,false);
					}
					break;

				case VK_F2:
					if(fullscreen == true)
					{
						ChangeDisplayMode(0,0,true);
						fullscreen = false; // No longer in full screen mode
						ShowCursor(TRUE);
					}
					break;
				
				case VK_F4:
					Init(hwnd);
					break;
				case VK_F5:
					pause = !pause;
					if (pause)
					{
						MessageBox(hwnd, "Paused", "Paused", MB_OK);
					}
					break;
				case VK_F6:
					distRemain = 500000;
					break;
				case VK_ESCAPE:
					
					// Tell the window we want to QUIT
					PostQuitMessage(0);
					break;
			}
	}													

	// Handle the default messages
	return DefWindowProc (hwnd, iMsg, wParam, lParam);		
															
}

void Init(HWND hwnd)
{	
	totalDistance = 3000000;
	speedx = 10;
	speedy = 0;
	treeSpeed = 15;
	cloudSpeed = 5;
	junkSpeed = 10;
	moonSpeed = 5;
	dist = 0;
	time = 0;
	angleOfAttack = 0;
	fuel = 30000;
	score = 0;
	pause = false;
	stallFlag = false;	
	userControl = true;
	fullscreen = false;
	takeOff = false;
	crash = false;
	gameOverFlag = false;
	landed = false;
	nextScene = false;
	calcScore = false;
	renamed = false;
	dispScore = true;
	// Create our double buffering
	CreateDoubleBuffering(&gBuffer, hwnd);

	// Load the sprites
	LoadSprite(&shuttle, "shuttle.bmp", SHUTTLE_WIDTH, SHUTTLE_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&tree2, "tree.bmp", TREE_WIDTH, TREE_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&tree, "tree2.bmp", TREE_WIDTH2, TREE_HEIGHT2, SHUTTLE_FRAMES);
	LoadSprite(&cloud, "cloud.bmp", CLOUD_WIDTH, CLOUD_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&junk, "junk.bmp", JUNK_WID, JUNK_HGT, JUNK_FRAMES);
	LoadSprite(&moon, "moon.bmp", MOON_WID, MOON_HGT, SHUTTLE_FRAMES);
	LoadSprite(&crashBmp, "crash.bmp", TREE_WIDTH, TREE_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&gameOver, "gameover.bmp", OVER_WIDTH, OVER_HEIGHT,
				SHUTTLE_FRAMES);
	LoadSprite(&explode, "explode2.bmp", 126, 100,
						SHUTTLE_FRAMES);	
	LoadSprite(&stall, "Stall.bmp", TREE_WIDTH, TREE_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&dive, "Dive.bmp",  TREE_WIDTH, TREE_HEIGHT, SHUTTLE_FRAMES);
	LoadSprite(&land, "land.bmp",  115, 129, SHUTTLE_FRAMES);
	// Set the sprites' positions
	SetSpritePosition(&shuttle, 150, 150);
	SetSpritePosition(&tree, 500, 300);
	SetSpritePosition(&tree2, 50, 300);
	SetSpritePosition(&cloud, 550, 100);
	SetSpritePosition(&junk, 550, 200);
	SetSpritePosition(&moon, 50, 50);
	SetSpritePosition(&gameOver, 165, 125);
	SetSpritePosition(&crashBmp, 265, 0);
	SetSpritePosition(&land, 265, 0);
	SetSpritePosition(&stall, 265, 0);
	SetSpritePosition(&dive, 265, 0);
	// Set the direction of the sprites
	SetSpriteState(&shuttle, EAST);
	SetSpriteState(&tree, WEST);
	SetSpriteState(&tree2, WEST);
	SetSpriteState(&cloud, WEST);
	SetSpriteState(&junk, WEST);
	SetSpriteState(&moon, WEST);
	// Set the X and Y speeds of the sprites
	SetSpriteSpeed(&shuttle, speedx, 0);
	SetSpriteSpeed(&tree, treeSpeed, 0);
	SetSpriteSpeed(&tree2, treeSpeed, 0);
	SetSpriteSpeed(&cloud, cloudSpeed, 0);
	SetSpriteSpeed(&junk, junkSpeed, 0);
	SetSpriteSpeed(&moon, moonSpeed, 0);

	// Load the background image
	hBackground = LoadABitmap("background.bmp");
															
	// Set the backbuffer to white first
	ClearScreen(gBuffer.hdcBack, gBuffer.scrnRect, WHITE_BRUSH);			

	// Display the background image
	DisplayBitmap(&gBuffer, hBackground, 0, 0);
}

void DeInit()
{
	// Free the sprites we created
	FreeSprite(&shuttle);
	FreeSprite(&tree);
	FreeSprite(&junk);
	FreeSprite(&moon);
	FreeSprite(&tree2);
	FreeSprite(&cloud);
	FreeSprite(&explode);
	FreeSprite(&crashBmp);
	FreeSprite(&gameOver);

	// Free the background image
	DeleteObject(hBackground);

	// Free the back buffer
	FreeBuffers(&gBuffer);

	// Post the QUIT message to the window														
	PostQuitMessage(0);				
}

bool ChangeDisplayMode(int pixel_width, int pixel_height, bool switch_back)
{
	// If we're switching back to our normal resolution
	if(switch_back)
	{
		ChangeDisplaySettings(NULL,0);
			return true;
	}

	DEVMODE dev_mode = {0};
	LONG result;

	// Get the current display settings
	if(!EnumDisplaySettings(NULL,ENUM_CURRENT_SETTINGS,&dev_mode))
		return false;

	// Change the DEVMODE to new settings
	memset(&dev_mode,0,sizeof(dev_mode));
	dev_mode.dmSize=sizeof(dev_mode);	
	dev_mode.dmPelsWidth = pixel_width;
	dev_mode.dmPelsHeight = pixel_height;
	dev_mode.dmBitsPerPel	= 16;
	dev_mode.dmFields = DM_BITSPERPEL|DM_PELSWIDTH | DM_PELSHEIGHT; 
												
	
	// Try to change to the new settings
	result = ChangeDisplaySettings(&dev_mode,CDS_FULLSCREEN);

	// ChangeDisplaySettings() returns DISP_CHANGE_SUCCESSFUL when it's successful
	if(result == DISP_CHANGE_SUCCESSFUL)
		return true;
	else
		return false; // Don't change to full screen
}

void PrintInfo(HWND hwnd, BUFFER *pBuffer, int distance, int speed, int fuel, int angle,
			   int time, bool gameOverFlag, bool landedFlag, int score)
{
	RECT rect;
	GetClientRect(hwnd, &rect);

	char buffDist[128] = {0};
	char buffSpeed[128] = {0};
	char buffFuel[128] = {0};
	char buffAngle[128] = {0};
	char buffTime[128] = {0};
	char finalScore[128] = {0};
	
	// To scale down the distance
	int totDist = distance / 100;
	// To scale up the speed
	int knotsSpeed = speed * 5;
	int aOa = angle * 2;

	sprintf(buffFuel, "Fuel remaining: %05d", fuel);
	sprintf(buffDist, "Distance remaining: %06d feet", totDist);
	sprintf(buffSpeed, "Speed: %03d kts", knotsSpeed);
	sprintf(buffAngle, "Angle of attack: %d degrees", aOa);
	sprintf(buffTime, "Game time: %03d", time);
	sprintf(finalScore, "Score: %04d", score);
						
	int centerX = 50;
	int centerY = 50;

	TEXTMETRIC textmetric;

	GetTextMetrics(pBuffer->hdcBack, &textmetric);

	int start = centerX - (6 * textmetric.tmAveCharWidth);

	TextOut(pBuffer->hdcBack, start, centerY - 20, buffDist, strlen(buffDist));
	TextOut(pBuffer->hdcBack, start, centerY - 40, buffSpeed, strlen(buffSpeed));
	TextOut(pBuffer->hdcBack, start, centerY + 40, buffFuel, strlen(buffFuel));
	TextOut(pBuffer->hdcBack, start, centerY, buffAngle, strlen(buffAngle));
	TextOut(pBuffer->hdcBack, start, centerY + 20, buffTime, strlen(buffTime));
	TextOut(pBuffer->hdcBack, start, centerY + 60, finalScore, strlen(finalScore));
}
