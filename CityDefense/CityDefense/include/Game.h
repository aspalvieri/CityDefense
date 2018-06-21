#ifndef GAME_H
#define GAME_H

#include "Texture.h"
#include "Sprite.h"
#include "Button.h"
#include "Tile.h"
#include "MapFunc.h"
#include "Object.h"

class MapFunc;

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
	void buildObjects();
	void destroy();

	//Main game
	void handleEvents();
	void update();
	void render();
	void cameraEvents(SDL_Event *e);
	void setCamera();
	void moveCamera();

	//Randomize
	int randomInt(int min, int max);
	double randomDouble(double min, double max);

	bool running();


//*************
//	Variables
//*************
	Sprite powerPlantSprite, reactorSprite, rockSprite, minerSprite;
	Object powerPlant, reactor, rocks, rockMiner;

	vector<Object*> objects;
	vector<tuple<Button*, Sprite, Object*>> buttonObjects;
	Button cancelButton, mapgenButton;
	Object *currentObject = NULL;
	bool hideObject = false, multiPlace = false, placedOnce = false;

	Button uiButton;

	Sprite deleteSprite;
	Object deleteObject;

	//Tilesheet
	Texture tilesForest, canPlace, cantPlace, gridline, uipanel, uibg;
	Tile *tiles[MAP_X][MAP_Y];
	int maxCamX, maxCamY, maxCamH, maxCamW;

	//Primary variable
	bool quit = false, up = false, down = false, right = false, left = false;
	SDL_Rect *camera;
	int camSpeed = 12;
	double xspeed = 0, yspeed = 0, posx, posy;

	//Random device
	random_device rd;
	mt19937 rng{rd()};
	MapFunc *mapfunc;

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