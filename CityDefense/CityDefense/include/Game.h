#ifndef GAME_H
#define GAME_H

#include "Texture.h"
#include "Sprite.h"
#include "Button.h"
#include "Tile.h"
#include "MapFunc.h"
#include "Object.h"

class MapFunc;
class Object;
class Ability;

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
	void buildTextDisplays();
	void buildObjects();
	void destroy();

	//Object Manipulation
	void addButtonObject(Sprite *spr, Object *obj);
	void addButtonObject(Sprite *spr, Object *obj, SDL_Rect req);
	void addButtonObject(Sprite *spr, Object *obj, Ability abi);
	void addButtonObject(Sprite *spr, Object *obj, SDL_Rect req, Ability abi);

	//Frames to Seconds
	double FTS(double time);

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
	Sprite powerPlantSprite, reactorSprite, rockSprite, goldRockSprite, minerSprite, farmSprite;
	Object powerPlant, reactor, rocks, goldrocks, rockMiner, smallFarm;

	vector<Object*> objects;
	vector<tuple<Button*, Sprite, Object*>> buttonObjects;
	Button cancelButton, mapgenButton, tileButton, deselectTarget;
	Object *currentObject = NULL, *targetObject = NULL, *preTargetObject = NULL, *preCurrentObject = NULL;
	bool hideObject = false, multiPlace = false, placedOnce = false;
	SDL_Rect emptyTile = TILE_NONE;

	Button uiButton;

	//Player variables
	int gold = BASE_GOLD, stone = BASE_STONE, population = BASE_POPULATION;
	int goldStorage = BASE_GOLD_STORAGE, stoneStorage = BASE_STONE_STORAGE, populationMax = BASE_POPULATION_MAX;
	//Variable displays
	Texture goldText, stoneText, populationText, tileHighlight, targetText, currentObjectText;
	stringstream tText;
	//Update variables
	int preGold = gold, preStone = stone, preGoldStorage = goldStorage, preStoneStorage = stoneStorage;
	int prePop = population, prePopMax = populationMax;

	//Tilesheet
	Texture tilesForest, canPlace, cantPlace, gridline, uipanel, uibg;
	Tile *tiles[MAP_X][MAP_Y];
	int maxCamX, maxCamY, maxCamH, maxCamW;

	//Primary variable
	bool quit = false, up = false, down = false, right = false, left = false;
	SDL_Rect *camera;
	int camSpeed = 12;
	double xspeed = 0, yspeed = 0, posx, posy;
	int bobx = 32, boby = 100;

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