#ifndef OBJECT_H
#define OBJECT_H

#include "Sprite.h"

class Object
{
public:
	Object();
	Object(const Object &o, bool addToManager = true);
	~Object();

	void move(double x, double y);
	void moveToMouse(double x, double y, bool snapToGrid = true);
	bool getCollide();

//private:
	Sprite self;
	bool canPlace = true, collide;
};

#endif//OBJECT_H