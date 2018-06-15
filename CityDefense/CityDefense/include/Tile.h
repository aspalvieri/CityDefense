#ifndef TILE_H
#define TILE_H

#include "Texture.h"

class Tile 
{
public:
	Tile();
	Tile(Texture *tilesheet, SDL_Rect c, int x, int y, bool col);
	~Tile();

	void draw();
	bool getCollide();

private:
	bool collide;
	Texture * tilesheet;
	SDL_Rect clip, *camera;
	int x, y;
};

#endif//TILE_H