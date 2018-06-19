#include "../include/Tile.h"

Tile::Tile()
{
}

Tile::Tile(Texture * tilesheet, SDL_Rect c, int x, int y, bool col, bool walk)
{
	this->tilesheet = tilesheet;
	clip = c;
	this->x = x;
	this->y = y;
	this->camera = &SDLR::camera;
	this->collide = col;
	this->walkable = walk;
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

bool Tile::operator==(const Tile & other)
{
	if (clip.x == other.clip.x && clip.y == other.clip.y && clip.w == other.clip.w && clip.h == other.clip.h)
		return true;
	return false;
}

bool Tile::operator==(const SDL_Rect & other)
{
	if (clip.x == other.x && clip.y == other.y && clip.w == other.w && clip.h == other.h)
		return true;
	return false;
}

bool Tile::operator!=(const SDL_Rect & other)
{
	if (clip.x == other.x && clip.y == other.y && clip.w == other.w && clip.h == other.h)
		return false;
	return true;
}
