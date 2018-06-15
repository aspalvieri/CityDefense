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
	for (unsigned int i = 0; i < game->objects.size(); i++) {
		game->objects[i]->self.removeFromManager();
	}
	if (!game->objects.empty())
		game->objects.clear();

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
			game->tiles[x][y] = new Tile(tilesheet, TILE_GRASS, x * TILE_SIZE, y * TILE_SIZE, false);
		}
	}

	//Spawn bodies of water
	int totalbodies = game->randomInt(45, 50);
	for (int i = 0; i < totalbodies; i++) {
		int waterspotx = game->randomInt(0, MAP_X - 1);
		int waterspoty = game->randomInt(0, MAP_Y - 1);
		if (game->tiles[waterspotx][waterspoty]->getCollide()) {
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
					if (size == smax && (x != wsx || y != wsy) && (x != wsw || y != wsh) && (x != wsw || y != wsy) && (x != wsx || y != wsh)) {
						delete game->tiles[x][y];
						game->tiles[x][y] = NULL;
						game->tiles[x][y] = new Tile(tilesheet, TILE_WATER, x * TILE_SIZE, y * TILE_SIZE, true);
					}
				}
			}
			size--;
		}
	}

	//Spawn rocks
	int totalrocks = game->randomInt(100, 150);
	while (totalrocks > 0) {
		int spx = game->randomInt(0, MAP_X - 1);
		int spy = game->randomInt(0, MAP_Y - 1);
		if (!game->tiles[spx][spy]->getCollide()) {
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
