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

//private:
	Sprite self;
};

#endif//OBJECT_H