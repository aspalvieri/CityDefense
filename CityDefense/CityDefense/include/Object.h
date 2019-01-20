#ifndef OBJECT_H
#define OBJECT_H

#include "Sprite.h"
#include "Tile.h"
#include "Button.h"

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

	//gi{Gold Income}, si{Stone Income}
	Object & setIncome(int gi = 0, int si = 0);

	//Subtracts object's costs from player's values
	Object & removeCost(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest, int *goldinc, int *stoneinc);

	/*Does the reverse of removeCost (Adds the object's costs to the player's values),
	and sets the object to be deleted on the next update*/
	Object & deleteObject(int *pop, int *popmax, int *gold, int *goldst, int *stone, int *stonest, int *goldinc, int *stoneinc);

	/*Returns true if the player meets the object's costs, 
	p{Population}, pm{Population Max}, g{Gold}, s{Stone}*/
	bool hasCost(int *p, int *pm, int *g, int *s);

	bool getCollide();

//private:
	int goldCost = 0, stoneCost = 0;
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
	int goldStorage = 0, stoneStorage = 0, population = 0, populationMax = 0;
	int goldIncome = 0, stoneIncome = 0;
};

#endif//OBJECT_H