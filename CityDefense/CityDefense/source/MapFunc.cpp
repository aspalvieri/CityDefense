#include "../include/MapFunc.h"

MapFunc::MapFunc()
{
}

MapFunc::MapFunc(Game * game)
{
	this->game = game;
}

void MapFunc::clearMap()
{
	//Clear pointer objects
	delete game->currentObject;
	game->currentObject = NULL;
	game->targetObject = NULL;
	game->preTargetObject = NULL;
	game->preCurrentObject = NULL;
	game->cancelButton.setVisible(false);

	//Clear objects vector
	for (auto & object : game->objects) {
		object->tileButton.removeFromManager();
		delete object;
		object = NULL;
	}
	if (!game->objects.empty())
		game->objects.clear();

	//Clear map tiles
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			delete game->tiles[x][y];
			game->tiles[x][y] = NULL;
		}
	}

	//Reset values
	game->gold = BASE_GOLD;
	game->stone = BASE_STONE;
	game->goldStorage = BASE_GOLD_STORAGE;
	game->stoneStorage = BASE_STONE_STORAGE;
	game->population = BASE_POPULATION;
	game->populationMax = BASE_POPULATION_MAX;
}

void MapFunc::generateMap(Texture * tilesheet)
{
	clearMap();

	//Fill map with water
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			game->tiles[x][y] = new Tile(tilesheet, TILE_WATER, x * TILE_SIZE, y * TILE_SIZE, true, false);
		}
	}

	//Spawn bodies of grass
	int totalbodies = game->randomInt(150, 175);
	//Pre-define the tiles because operator overload won't allow defines
	SDL_Rect shore = TILE_SHORE;
	SDL_Rect water = TILE_WATER;
	SDL_Rect grass = TILE_GRASS;
	SDL_Rect sand = TILE_SAND;
	SDL_Rect dirt = TILE_DIRT;
	bool first = true;
	for (int i = 0; i < totalbodies; i++) {
		int size = game->randomInt(2, 4);
		int smax = size;

		int waterspotx = game->randomInt(2 + size, MAP_X - size - 3);
		int waterspoty = game->randomInt(2 + size, MAP_Y - size - 3);
		if (!game->tiles[waterspotx][waterspoty]->getCollide()) {
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
					delete game->tiles[x][y];
					game->tiles[x][y] = NULL;
					game->tiles[x][y] = new Tile(tilesheet, TILE_GRASS, x * TILE_SIZE, y * TILE_SIZE, false, true);
				}
			}
			size--;
		}
	}

	//Add sand line
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			if (*game->tiles[x][y] == grass) {
				if ((x > 0 && *game->tiles[x - 1][y] == water)
					|| (x < MAP_X-1 && *game->tiles[x + 1][y] == water)
					|| (y > 0 && *game->tiles[x][y - 1] == water)
					|| (y < MAP_Y-1 && *game->tiles[x][y + 1] == water))
				{
					delete game->tiles[x][y];
					game->tiles[x][y] = NULL;
					game->tiles[x][y] = new Tile(tilesheet, TILE_SAND, x * TILE_SIZE, y * TILE_SIZE, false, true);
				}
			}
		}
	}

	//Add shore line
	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			if (*game->tiles[x][y] == water) {
				if ((x > 0 && *game->tiles[x - 1][y] == sand)
					|| (x < MAP_X - 1 && *game->tiles[x + 1][y] == sand)
					|| (y > 0 && *game->tiles[x][y - 1] == sand)
					|| (y < MAP_Y - 1 && *game->tiles[x][y + 1] == sand))
				{
					delete game->tiles[x][y];
					game->tiles[x][y] = NULL;
					game->tiles[x][y] = new Tile(tilesheet, TILE_SHORE, x * TILE_SIZE, y * TILE_SIZE, true, true);
				}
			}
		}
	}

	//Add dirt
	int totaldirt = game->randomInt(25, 30);
	for (int i = 0; i < totaldirt; i++) {
		int size = game->randomInt(1, 1);
		int smax = size;

		int waterspotx = game->randomInt(2 + size, MAP_X - size - 3);
		int waterspoty = game->randomInt(2 + size, MAP_Y - size - 3);
		if (*game->tiles[waterspotx][waterspoty] != grass) {
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
					if (*game->tiles[x][y] == grass) {
						delete game->tiles[x][y];
						game->tiles[x][y] = NULL;
						game->tiles[x][y] = new Tile(tilesheet, TILE_DIRT, x * TILE_SIZE, y * TILE_SIZE, false, true);
					}
				}
			}
			size--;
		}
	}

	//Spawn rocks
	int totalrocks = game->randomInt(100, 125);
	while (totalrocks > 0) {
		int spx = game->randomInt(0, MAP_X - 1);
		int spy = game->randomInt(0, MAP_Y - 1);
		if (!game->tiles[spx][spy]->getCollide() && *game->tiles[spx][spy] == grass) {
			game->rocks.self.setPosition(spx * TILE_SIZE, spy * TILE_SIZE);
			bool hit = false;
			for (unsigned int i = 0; i < game->objects.size(); i++) {
				if (game->objects[i]->getCollide() && checkCollision(&game->rocks.self.getBox(), &game->objects[i]->self.getBox())) {
					hit = true;
				}
			}
			if (!hit) {
				game->rocks.abilityValue = game->randomInt(ROCK_VALUE_MIN, ROCK_VALUE_MAX);
				game->objects.push_back(new Object(game->rocks));
				game->objects.back()->self.setFrame(game->randomInt(0, 3));
				game->objects.back()->tileButton.setVisible(true);
				totalrocks--;
				continue;
			}
		}
	}

	//Spawn gold rocks
	totalrocks = game->randomInt(40, 60);
	while (totalrocks > 0) {
		int spx = game->randomInt(0, MAP_X - 1);
		int spy = game->randomInt(0, MAP_Y - 1);
		if (!game->tiles[spx][spy]->getCollide() && *game->tiles[spx][spy] == grass) {
			game->goldrocks.self.setPosition(spx * TILE_SIZE, spy * TILE_SIZE);
			bool hit = false;
			for (unsigned int i = 0; i < game->objects.size(); i++) {
				if (game->objects[i]->getCollide() && checkCollision(&game->goldrocks.self.getBox(), &game->objects[i]->self.getBox())) {
					hit = true;
				}
			}
			if (!hit) {
				game->goldrocks.abilityValue = game->randomInt(GOLD_ROCK_VALUE_MIN, GOLD_ROCK_VALUE_MAX);
				game->objects.push_back(new Object(game->goldrocks));
				game->objects.back()->self.setFrame(game->randomInt(0, 3));
				game->objects.back()->tileButton.setVisible(true);
				totalrocks--;
				continue;
			}
		}
	}
}
