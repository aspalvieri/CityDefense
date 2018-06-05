#ifndef TILE_H
#define TILE_H

#include "Texture.h"

class Tile 
{
public:
	Tile();
	Tile(Texture *tilesheet, SDL_Rect c, int x, int y);
	~Tile();

	void draw();

private:
	Texture * tilesheet;
	SDL_Rect clip, *camera;
	int x, y;
};

#endif//TILE_H