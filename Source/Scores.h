#ifndef _SCORES_H					
#define _SCORES_H	

#include "main.h"
#include <list>
#include <string.h>
#include <fstream>

// Returns true if the player has achieved a high score
bool CalculateList(int score, BUFFER *pBuffer, bool& renamed);
// Outouts the high score list
void outputScores(HWND hwnd);
#endif