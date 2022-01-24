#ifndef BUTTON_H
#define BUTTON_H

#include "Texture.h"

class Button
{
public:
	Button();
	~Button();

	static vector<Button*> buttonManager;

	Button& addToManager();

	Button& removeFromManager();

	Button& setText(string t, SDL_Color = { 255,255,255 }, TTF_Font* = NULL);

	Button& setSize(double w, double h);
	
	Button& setPosition(double xpos, double ypos);

	Button& setImage(string path);

	Button& setObjectButton(bool o);
	bool getObjectButton();

	Button& setImage(Texture img);
	Texture& getImage();

	Button& setCamera(SDL_Rect *cam = NULL);

	Button& setVisible(bool v);
	bool getVisible();

	bool Pressed();
	void forceRefresh();

	void updateText(string t);

	void handleEvents(SDL_Event *e);

	void render(int nx = 0, int ny = 0);
	
private:
	SDL_Rect * camera = NULL;
	void refresh();
	bool released = false;
	bool visible = true;
	bool inManager = false;
	bool objectButton = false;
	Texture image, name;
	pair<int, int> *mousePos;
	int *mouseButton;
	bool pressed = false;

	double x = 0, y = 0, width = 200, height = 100, bx = 0, by = 0;
	string text = "";

	SDL_Rect clip;
	TTF_Font* lastFont = NULL;
	SDL_Color lastColor = { 255,255,255 };

	SDL_Renderer* gRenderer = NULL;
};

#endif//BUTTON_H