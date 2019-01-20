#include "../include/Game.h"

void Game::clearMap()
{
	//Clear pointer objects
	delete currentObject;
	currentObject = NULL;
	targetObject = NULL;
	preTargetObject = NULL;
	preCurrentObject = NULL;
	cancelButton.setVisible(false);

	//Clear objects vector
	for (auto & object : objects) {
		object->tileButton.removeFromManager();
		delete object;
		object = NULL;
	}
	if (!objects.empty())
		objects.clear();

	//Clear map tiles
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			delete tiles[x][y];
			tiles[x][y] = NULL;
		}
	}

	//Reset values
	gold = BASE_GOLD;
	stone = BASE_STONE;
	goldStorage = BASE_GOLD_STORAGE;
	stoneStorage = BASE_STONE_STORAGE;
	population = BASE_POPULATION;
	populationMax = BASE_POPULATION_MAX;
	goldIncome = 0;
	stoneIncome = 0;
}

void Game::generateMap(Texture * tilesheet)
{
	clearMap();

	//Fill map with water
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			tiles[x][y] = new Tile(tilesheet, TILE_WATER, x * TILE_SIZE, y * TILE_SIZE, true, false);
		}
	}

	//Spawn bodies of grass
	int totalbodies = randomInt(150, 175);
	//Pre-define the tiles because operator overload won't allow defines
	SDL_Rect shore = TILE_SHORE;
	SDL_Rect water = TILE_WATER;
	SDL_Rect grass = TILE_GRASS;
	SDL_Rect sand = TILE_SAND;
	SDL_Rect dirt = TILE_DIRT;
	bool first = true;
	for (int i = 0; i < totalbodies; i++) {
		int size = randomInt(2, 4);
		int smax = size;

		int waterspotx = randomInt(2 + size, MAP_X - size - 3);
		int waterspoty = randomInt(2 + size, MAP_Y - size - 3);
		if (!tiles[waterspotx][waterspoty]->getCollide()) {
			i--;
			continue;
		}
		while (size != 0) {
			int wsx = ((waterspotx - size) > 0) ? waterspotx - size : 0;
			int wsy = ((waterspoty - size) > 0) ? waterspoty - size : 0;
			int wsw = ((waterspotx + size) < MAP_X) ? waterspotx + size : MAP_X - 1;
			int wsh = ((waterspoty + size) < MAP_Y) ? waterspoty + size : MAP_Y - 1;
			for (int y = wsy; y <= wsh; y++) {
				for (int x = wsx; x <= wsw; x++) {
					delete tiles[x][y];
					tiles[x][y] = NULL;
					tiles[x][y] = new Tile(tilesheet, TILE_GRASS, x * TILE_SIZE, y * TILE_SIZE, false, true);
				}
			}
			size--;
		}
	}

	//Add sand line
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			if (*tiles[x][y] == grass) {
				if ((x > 0 && *tiles[x - 1][y] == water)
					|| (x < MAP_X-1 && *tiles[x + 1][y] == water)
					|| (y > 0 && *tiles[x][y - 1] == water)
					|| (y < MAP_Y-1 && *tiles[x][y + 1] == water))
				{
					delete tiles[x][y];
					tiles[x][y] = NULL;
					tiles[x][y] = new Tile(tilesheet, TILE_SAND, x * TILE_SIZE, y * TILE_SIZE, false, true);
				}
			}
		}
	}

	//Add shore line
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			if (*tiles[x][y] == water) {
				if ((x > 0 && *tiles[x - 1][y] == sand)
					|| (x < MAP_X - 1 && *tiles[x + 1][y] == sand)
					|| (y > 0 && *tiles[x][y - 1] == sand)
					|| (y < MAP_Y - 1 && *tiles[x][y + 1] == sand))
				{
					delete tiles[x][y];
					tiles[x][y] = NULL;
					tiles[x][y] = new Tile(tilesheet, TILE_SHORE, x * TILE_SIZE, y * TILE_SIZE, true, true);
				}
			}
		}
	}

	/*
	//Dirt, rocks, and gold

	//Add dirt
	int totaldirt = randomInt(25, 30);
	for (int i = 0; i < totaldirt; i++) {
		int size = randomInt(1, 1);
		int smax = size;

		int waterspotx = randomInt(2 + size, MAP_X - size - 3);
		int waterspoty = randomInt(2 + size, MAP_Y - size - 3);
		if (*tiles[waterspotx][waterspoty] != grass) {
			i--;
			continue;
		}
		while (size != 0) {
			int wsx = ((waterspotx - size) > 0) ? waterspotx - size : 0;
			int wsy = ((waterspoty - size) > 0) ? waterspoty - size : 0;
			int wsw = ((waterspotx + size) < MAP_X) ? waterspotx + size : MAP_X - 1;
			int wsh = ((waterspoty + size) < MAP_Y) ? waterspoty + size : MAP_Y - 1;
			for (int y = wsy; y <= wsh; y++) {
				for (int x = wsx; x <= wsw; x++) {
					if (*tiles[x][y] == grass) {
						delete tiles[x][y];
						tiles[x][y] = NULL;
						tiles[x][y] = new Tile(tilesheet, TILE_DIRT, x * TILE_SIZE, y * TILE_SIZE, false, true);
					}
				}
			}
			size--;
		}
	}

	//Spawn rocks
	int totalrocks = randomInt(100, 125);
	while (totalrocks > 0) {
		int spx = randomInt(0, MAP_X - 1);
		int spy = randomInt(0, MAP_Y - 1);
		if (!tiles[spx][spy]->getCollide() && *tiles[spx][spy] == grass) {
			rocks.self.setPosition(spx * TILE_SIZE, spy * TILE_SIZE);
			bool hit = false;
			for (unsigned int i = 0; i < objects.size(); i++) {
				if (objects[i]->getCollide() && checkCollision(&rocks.self.getBox(), &objects[i]->self.getBox())) {
					hit = true;
				}
			}
			if (!hit) {
				rocks.abilityValue = randomInt(ROCK_VALUE_MIN, ROCK_VALUE_MAX);
				objects.push_back(new Object(rocks));
				objects.back()->self.setFrame(randomInt(0, 3));
				objects.back()->tileButton.setVisible(true);
				totalrocks--;
				continue;
			}
		}
	}

	//Spawn gold rocks
	totalrocks = randomInt(40, 60);
	while (totalrocks > 0) {
		int spx = randomInt(0, MAP_X - 1);
		int spy = randomInt(0, MAP_Y - 1);
		if (!tiles[spx][spy]->getCollide() && *tiles[spx][spy] == grass) {
			goldrocks.self.setPosition(spx * TILE_SIZE, spy * TILE_SIZE);
			bool hit = false;
			for (unsigned int i = 0; i < objects.size(); i++) {
				if (objects[i]->getCollide() && checkCollision(&goldrocks.self.getBox(), &objects[i]->self.getBox())) {
					hit = true;
				}
			}
			if (!hit) {
				goldrocks.abilityValue = randomInt(GOLD_ROCK_VALUE_MIN, GOLD_ROCK_VALUE_MAX);
				objects.push_back(new Object(goldrocks));
				objects.back()->self.setFrame(randomInt(0, 3));
				objects.back()->tileButton.setVisible(true);
				totalrocks--;
				continue;
			}
		}
	}
	*/
}
