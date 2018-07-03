#ifndef OBJECT_H
#define OBJECT_H

#include "Sprite.h"
#include "Tile.h"
#include "Button.h"

class Object;

//Object event
class Ability
{
public:
	Ability() {}
	Ability(string t, int *gval, int *sval, int inc, int spd) {
		alive = true;
		income = inc;
		speed = spd;
		type = t;
		intValues.push_back(gval);
		intValues.push_back(sval);
	}
	Ability(string t, int *gval, int *sval, int inc, int spd, int rem) : Ability(t, gval, sval, inc, spd) {
		remaining = rem;
	}
	~Ability() {}

	void run();
	string printAbility();

	bool running = true, enabled = false, alive = false;

	string type = "";
	vector<int*> intValues;
	Object* objectValue = NULL;
	int income = 0, speed = 0, timer = 0, remaining = 0;
};

class Object
{
public:
	Object();
	Object(const Object &o, bool addToManager = true);
	~Object();

	void move(double x, double y);
	void moveToMouse(double x, double y, bool snapToGrid = true);
	Object & setCost(string n = "", int pop = 0, int gc = 0, int sc = 0);
	Object & setButton(Button b, Texture *h);
	Object & setStorage(int ps = 0, int gs = 0, int ss = 0);
	Object & removeCost(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest);
	Object & deleteObject(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest);
	bool hasCost(int *p, int *pm, int *g, int *s);
	bool getCollide();
	void runAbility();

//private:
	int goldCost = 0, stoneCost = 0;
	Ability ability;
	Button tileButton;
	Sprite self;
	string type = "", subtype = "", requiredType = "", name = "";
	Tile requiredTile;
	Texture *highlight = NULL;
	bool canPlace = true, collide, defaultPlace = true, setToDelete = false;
	int abilityValue = 0, goldStorage = 0, stoneStorage = 0, population = 0, populationMax;
};

#endif//OBJECT_H