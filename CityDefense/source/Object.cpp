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
		tileButton.addToManager()
			.setPosition(self.getBox().x, self.getBox().y)
			.setSize(self.getBox().w, self.getBox().h)
			.setCamera(self.getCamera());
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
		bool hit = false;
		if (self.getBox().x + self.getBox().w > self.getCamera()->w + self.getCamera()->x) {
			self.setPosition(self.getCamera()->w + self.getCamera()->x - self.getBox().w, self.getBox().y);
			hit = true;
		}
		if (self.getBox().y + self.getBox().h > self.getCamera()->h + self.getCamera()->y) {
			self.setPosition(self.getBox().x, self.getCamera()->h + self.getCamera()->y - self.getBox().h);
			hit = true;
		}
		if (hit) {
			negx = self.getBox().x % TILE_SIZE;
			negy = self.getBox().y % TILE_SIZE;
			self.setPosition(self.getBox().x - negx, self.getBox().y - negy);
		}
	}
	else {
		self.setPosition(x - self.getCenter().x, y - self.getCenter().y);
	}
}

Object & Object::setCost(string n, int pop, int gc, int sc)
{
	name = n;
	population = pop;
	goldCost = gc;
	stoneCost = sc;
	return *this;
}

Object & Object::setButton(Button b, Texture *h)
{
	highlight = new Texture(*h);
	tileButton = b;
	highlight->setSize(self.getBox().w, self.getBox().h);
	return *this;
}

Object & Object::setStorage(int ps, int gs, int ss)
{
	populationMax = ps;
	goldStorage = gs;
	stoneStorage = ss;
	return *this;
}

Object & Object::setIncome(int gi, int si)
{
	goldIncome = gi;
	stoneIncome = si;
	return *this;
}

Object & Object::removeCost(int * pop, int * popmax, int * gold, int * goldst, int * stone, int * stonest, int * goldinc, int * stoneinc)
{
	*pop += population;
	*popmax += populationMax;
	*gold -= goldCost;
	*goldst += goldStorage;
	*stone -= stoneCost;
	*stonest += stoneStorage;
	*goldinc += goldIncome;
	*stoneinc += stoneIncome;
	return *this;
}

Object & Object::deleteObject(int * pop, int * popmax, int * gold, int * goldst, int * stone, int * stonest, int * goldinc, int * stoneinc)
{
	*pop -= population;
	*popmax -= populationMax;
	*gold += goldCost;
	*goldst -= goldStorage;
	*stone += stoneCost;
	*stonest -= stoneStorage;
	*goldinc -= goldIncome;
	*stoneinc -= stoneIncome;
	setToDelete = true;
	tileButton.removeFromManager();
	return *this;
}

bool Object::hasCost(int *p, int *pm, int * g, int * s)
{
	if (*g >= goldCost && *s >= stoneCost && *p + population <= *pm)
		return true;
	return false;
}

bool Object::getCollide()
{
	return collide;
}
