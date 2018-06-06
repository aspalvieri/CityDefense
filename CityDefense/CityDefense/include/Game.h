#ifndef GAME_H
#define GAME_H

#include "Texture.h"
#include "Sprite.h"
#include "Button.h"
#include "Tile.h"

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
	void destroy();

	//Main game
	void handleEvents();
	void update();
	void render();
	void setCamera();
	void moveCamera();

	bool running();

private:
	Button a, b;
	Sprite g;

	//Tilesheet
	Texture tilesForest;
	Tile *tiles[MAP_X][MAP_Y];
	int maxCamX, maxCamY, maxCamH, maxCamW;

	//Primary variable
	bool quit = false, up = false, down = false, right = false, left = false;
	SDL_Rect *camera;
	int camSpeed = 8;

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