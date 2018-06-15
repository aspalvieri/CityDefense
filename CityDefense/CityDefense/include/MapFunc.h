#ifndef MAPFUNC_H
#define MAPFUNC_H

#include "Game.h"

class Game;

class MapFunc {
public:
	MapFunc();
	MapFunc(Game *game);

	void clearMap();

	void generateMap(Texture * tilesheet);

private:
	Game * game;
};



#endif//MAPFUN_H