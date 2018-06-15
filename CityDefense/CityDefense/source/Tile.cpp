#include "../include/Tile.h"

Tile::Tile()
{
}

Tile::Tile(Texture * tilesheet, SDL_Rect c, int x, int y, bool col)
{
	this->tilesheet = tilesheet;
	clip = c;
	this->x = x;
	this->y = y;
	this->camera = &SDLR::camera;
	this->collide = col;
}

Tile::~Tile()
{
}

void Tile::draw()
{
	tilesheet->render(x - camera->x, y - camera->y, &clip);
}

bool Tile::getCollide()
{
	return collide;
}

SDL_Rect Tile::getBox()
{
	return { x, y, clip.w, clip.h };
}
