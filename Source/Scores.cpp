#include "Scores.h"
using namespace std;

// Loads up the scores from file into an STL list,
// then compares the player's score to these. If player
// has achieved a high score, it is added to the list
// and the lowest score in the list is dropped.
bool CalculateList(int score, BUFFER *pBuffer, bool& renamed)
{
	ifstream fin;
	ofstream fout;
	bool highScore = false;
	int newScore;
	
	list<int> scoreList;
	list<int>:: iterator i;

	// Open the high score file
	fin.open("highscores.txt");
	if(fin.fail())
	{
		TextOut(pBuffer->hdcBack, 50, 200, "FAILED TO OPEN FILE", 20);
	}

	while(fin.peek() != EOF)
	{
		// Put the current score from the file into the list
		fin >> newScore;
		scoreList.push_back(newScore);
	}
	fin.close();

	// Open another file
	fout.open("highscores2.txt");
	if(fout.fail())
	{
		TextOut(pBuffer->hdcBack, 50, 200, "FAILED TO OPEN FILE", 20);
	}

	int length = scoreList.size();

	// If there are 10 scores or less in the list
	if(length >= 10)
	{
		for(i = scoreList.begin(); i != scoreList.end(); i++)
		{
			if (score > *i && !highScore)
			{
				// If the score is higher than any of the scores in
				// the list, push it onto the list and pop the lowest
				// score off the list
				scoreList.pop_back();
				scoreList.push_front(score);
				highScore = true;
			}
		}
	}
	// If there are 10 or less scores in the list
	else if(length < 11)
	{
		// Push the score onto the list
		scoreList.push_front(score);
		scoreList.pop_back();
		highScore = true;
	}

	length = scoreList.size();

	// Make sure there are 10 scores in the list
	int ctr = length - 10;
	if(length > 10)
	{
		while (ctr != 0)
		{
			scoreList.pop_back();
			ctr--;
		}
	}
	// Sort the list
	scoreList.sort();
	// Reverse the list so the scores are in descending order
	scoreList.reverse();

	for(i = scoreList.begin(); i != scoreList.end(); i++)
	{
		// Read out the list to file
		fout << *i << endl;
	}
	
	fout.close();

	// Delete highscores.txt and rename highscores2.txt to
	// highscores.txt
	remove("highscores.txt");
	rename("highscores2.txt", "highscores.txt");

	// Return boolean
	return highScore;
}

// Outputs the high scores in a message box.
void outputScores(HWND hwnd)
{
	ifstream fin;
	int newScore;

	list<int> scoreList;
	list<int>:: iterator i;

	fin.open("highscores.txt");

	while(fin.peek() != EOF)
	{
		// Read the high scores into the list
		fin >> newScore;
		scoreList.push_back(newScore);
	}
	// Pop the back off the list
	scoreList.pop_back();
	fin.close();

	char text[128] = {0};
	char score[128] = {0};
	int counter = 1;

	for(i= scoreList.begin(); i != scoreList.end(); i++)
	{
		// Put the scores into a string along with their high score
		// ranking
		sprintf(score, "%d: %d \n", counter, *i);
		strncat(text, score, 128);
		counter++;
	}

	// Output the score list
	MessageBox(hwnd,text,"High Scores",MB_OK);

	fin.close();
}
