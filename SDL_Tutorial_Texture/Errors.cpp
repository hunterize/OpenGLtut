#include "errors.h"
#include <iostream>
#include <SDL\SDL.h>
using namespace std;


void fatalError(string errorString)
{
	cout << errorString << endl;
	cout << "Enter any key to quit...";
	int tmp;
	cin >> tmp;
	SDL_Quit();
	exit(1);
}