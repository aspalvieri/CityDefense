#ifndef GAME_H
#define GAME_H

#include "Object.h"
#include "Timer.h"

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

	//Frames to Seconds
	int FTS(double time);

	//Main game
	void handleEvents();
	void update();
	void render();
	void cameraEvents(SDL_Event *e);
	void setCamera();
	void moveCamera();

	//Map Functions
	void clearMap();
	void generateMap(Texture * tilesheet);

	//Randomize
	int randomInt(int min, int max);
	double randomDouble(double min, double max);

	bool running();


//*************
//	Variables
//*************
	Sprite powerPlantSprite, reactorSprite, farmSprite, basicTowerSprite;
	Object powerPlant, reactor, smallFarm, basicTower;

	vector<Object*> objects;
	vector<tuple<Button*, Sprite, Object*>> buttonObjects;
	Button cancelButton, mapgenButton, tileButton, deselectTarget;
	Object *currentObject = NULL, *targetObject = NULL, *preTargetObject = NULL, *preCurrentObject = NULL;
	bool hideObject = false, multiPlace = false, placedOnce = false;
	SDL_Rect emptyTile = TILE_NONE;

	Button uiButton;

	//Player variables
	int goldIncome = 0, stoneIncome = 0;
	int gold = BASE_GOLD, stone = BASE_STONE, population = BASE_POPULATION;
	int goldStorage = BASE_GOLD_STORAGE, stoneStorage = BASE_STONE_STORAGE, populationMax = BASE_POPULATION_MAX;
	//Variable displays
	Texture goldText, stoneText, populationText, tileHighlight, buttonHighlight, targetText, fpsText;
	Texture currentObjectTextName, currentObjectTextGoldCost, currentObjectTextStoneCost, currentObjectTextPopulationCost, currentObjectTextInfo;
	stringstream tText;
	Timer fpsTimer, capFpsTimer;
	double averageFPS = 0;
	int countedFrames = 0;
	//Update variables
	int preGold = gold, preStone = stone, preGoldStorage = goldStorage, preStoneStorage = stoneStorage;
	string preAmp = "";
	int preGoldIncome = goldIncome, preStoneIncome = stoneIncome;
	int prePop = population, prePopMax = populationMax;

	//Tilesheet
	Texture tilesForest, canPlace, canPlacePoor, cantPlace, gridline, uipanel, uibg;
	Tile *tiles[MAP_X][MAP_Y];
	int maxCamX, maxCamY, maxCamH, maxCamW;

	//Primary variable
	int incomeFrames = 0;
	bool quit = false, up = false, down = false, right = false, left = false;
	SDL_Rect *camera;
	int camSpeed = 12;
	//Camera speeds and position
	double xspeed = 0, yspeed = 0, posx, posy;
	//Button object x and y
	int bobx = 32, boby = 100;

	//Random device
	random_device rd;
	mt19937 rng{rd()};

	//Static variables
	pair<int, int> *mousePos;
	int *mouseButton;
	map<int, TTF_Font*> fontManager;
	const SDL_Color COLOR_WHITE = { 255,255,255 };
	const SDL_Color COLOR_RED = { 255,0,0 };

	//SDL variables
	SDL_Event e;
	SDL_Renderer *gRenderer = NULL;
	SDL_Window *gWindow = NULL;
};

#endif//GAME_H