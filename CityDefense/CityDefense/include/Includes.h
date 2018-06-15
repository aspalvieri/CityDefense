#ifndef INCLUDES_H
#define INCLUDES_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <sstream>
#include <algorithm>
#include <time.h>
#include <random>

using namespace std;

//Circle structure
struct Circle
{
	int x, y;
	int r;
};

#endif//INCLUDES_H