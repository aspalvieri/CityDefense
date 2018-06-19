#ifndef TILE_H
#define TILE_H

#include "Texture.h"

class Tile 
{
public:
	Tile();
	Tile(Texture *tilesheet, SDL_Rect c, int x, int y, bool col, bool walk);
	~Tile();

	void draw();
	bool getCollide();
	SDL_Rect getBox();

	bool operator==(const Tile& other);
	bool operator==(const SDL_Rect& other);
	bool operator!=(const SDL_Rect& other);

private:
	bool collide, walkable;
	Texture * tilesheet;
	SDL_Rect clip, *camera;
	int x, y;
};

#endif//TILE_H