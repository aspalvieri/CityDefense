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

	//If set to true, forces the display to update the selected text
	bool abilityUpdated = false;

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
	
	//n{Name}, pop{Population}, gc{Gold Cost}, sc{Stone Cost}
	Object & setCost(string n = "", int pop = 0, int gc = 0, int sc = 0);

	//Set button for UI
	Object & setButton(Button b, Texture *h);

	//ps{Population Storage}, gs{Gold Storage}, ss{Stone Storage}
	Object & setStorage(int ps = 0, int gs = 0, int ss = 0);

	//Subtracts object's costs from player's values
	Object & removeCost(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest);

	/*Does the reverse of removeCost (Adds the object's costs to the player's values),
	and sets the object to be deleted on the next update*/
	Object & deleteObject(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest);

	/*Returns true if the player meets the object's costs, 
	p{Population}, pm{Population Max}, g{Gold}, s{Stone}*/
	bool hasCost(int *p, int *pm, int *g, int *s);
	bool getCollide();
	void runAbility();

//private:
	int goldCost = 0, stoneCost = 0;
	Ability ability;
	Button tileButton;
	Sprite self;
	string type = "", subtype = "", name = "";
	//The type of tile needed for the object to be placeable. If one is set, 'defaultPlace' has to be false
	string requiredType = "";
	Tile requiredTile;
	Texture *highlight = NULL;
	bool canPlace = true, collide, setToDelete = false;
	//If the object can be placed by default, setting false will require a 'requiredType'
	bool defaultPlace = true;
	//The int value in which the ability uses
	int abilityValue = 0;
	int goldStorage = 0, stoneStorage = 0, population = 0, populationMax;
};

#endif//OBJECT_H