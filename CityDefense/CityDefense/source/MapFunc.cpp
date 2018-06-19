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
	//Clear objects vector
	for (auto & object : game->objects) {
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
}

void MapFunc::generateMap(Texture * tilesheet)
{
	clearMap();

	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			game->tiles[x][y] = new Tile(tilesheet, TILE_WATER, x * TILE_SIZE, y * TILE_SIZE, true, false);
		}
	}

	//Spawn bodies of grass
	int totalbodies = game->randomInt(150, 175);
	SDL_Rect shore = TILE_SHORE;
	SDL_Rect water = TILE_WATER;
	SDL_Rect grass = TILE_GRASS;
	SDL_Rect sand = TILE_SAND;
	bool first = true;
	for (int i = 0; i < totalbodies; i++) {
		int waterspotx = game->randomInt(0, MAP_X - 1);
		int waterspoty = game->randomInt(0, MAP_Y - 1);
		if (!game->tiles[waterspotx][waterspoty]->getCollide()) {
			i--;
			continue;
		}
		int size = game->randomInt(2, 4);
		int smax = size;
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

	//Spawn rocks
	int totalrocks = game->randomInt(200, 250);
	while (totalrocks > 0) {
		int spx = game->randomInt(0, MAP_X - 2);
		int spy = game->randomInt(0, MAP_Y - 2);
		if (!game->tiles[spx][spy]->getCollide() && *game->tiles[spx][spy] == grass) {
			game->rocks.self.setPosition(spx * TILE_SIZE, spy * TILE_SIZE);
			bool hit = false;
			for (unsigned int i = 0; i < game->objects.size(); i++) {
				if (game->objects[i]->getCollide() && checkCollision(&game->rocks.self.getBox(), &game->objects[i]->self.getBox())) {
					hit = true;
				}
			}
			if (!hit) {
				game->objects.push_back(new Object(game->rocks));
				game->objects.back()->self.setFrame(game->randomInt(0, 2));
				totalrocks--;
				continue;
			}
		}
	}
}
