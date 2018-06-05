#include "../include/Tile.h"

Tile::Tile()
{
}

Tile::Tile(Texture * tilesheet, SDL_Rect c, int x, int y)
{
	this->tilesheet = tilesheet;
	clip = c;
	this->x = x;
	this->y = y;
	this->camera = &SDLR::camera;
}

Tile::~Tile()
{
}

void Tile::draw()
{
	tilesheet->render(x - camera->x, y - camera->y, &clip);
}
