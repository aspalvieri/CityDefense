#ifndef GAME_H
#define GAME_H

#include "Texture.h"
#include "Button.h"

class Game
{
public:
	//Constructor/destructor
	Game();
	~Game();

	//Run-time builds
	void initialize();
	void buildFontManager();
	void buildButtons();
	void buildImages();

	//Main game
	bool runGame();

private:
	Button a, b;

	//Primary variable
	bool quit = false;

	//Static variables
	pair<int, int> *mousePos;
	int *mouseButton;
	map<int, TTF_Font*> fontManager;

	//SDL variables
	SDL_Event e;
	SDL_Renderer *gRenderer = NULL;
	SDL_Window *gWindow = NULL;
};

#endif//GAME_H