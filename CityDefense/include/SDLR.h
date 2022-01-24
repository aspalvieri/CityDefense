#ifndef SDLR_H
#define SDLR_H

#include "Includes.h"
#include "Globals.h"

//Static SDL Renderer, easier to pass around
struct SDLR
{
	//Rendering software
	static SDL_Renderer* gRenderer;

	//Window pane
	static SDL_Window* gWindow;

	//User's mouse position
	static pair<int, int> mousePosition;

	//User's mouse button
	static int mouseButton;

	//Player's camera
	static SDL_Rect camera;

	//Mouse position as a box
	static SDL_Rect mPos;

	SDLR();
};

#endif//SDLR_H