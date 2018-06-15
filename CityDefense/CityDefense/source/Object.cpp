#include "../include/Object.h"

Object::Object()
{
	collide = true;
}

Object::Object(const Object & o, bool addToManager)
{
	*this = o;
	if (addToManager) {
		self.addToManager();
		self.setFrame(0);
		self.resetTicks();
	}
}

Object::~Object()
{
}

void Object::move(double x, double y)
{
	self.setPosition(x, y);
}

void Object::moveToMouse(double x, double y, bool snapToGrid)
{
	if (snapToGrid) {
		self.setPosition(x, y);
		int negx = self.getBox().x % TILE_SIZE;
		int negy = self.getBox().y % TILE_SIZE;
		self.setPosition(self.getBox().x - negx, self.getBox().y - negy);
	}
	else {
		self.setPosition(x - self.getCenter().x, y - self.getCenter().y);
	}
}

bool Object::getCollide()
{
	return collide;
}
