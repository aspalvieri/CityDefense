#ifndef SPRITE_H
#define SPRITE_H

#include "SDLR.h"
#include "Texture.h"

class Sprite
{
public:
	Sprite();
	~Sprite();

	//All sprites are added to this on creation, removed on deletion
	static vector<Sprite*> spriteManager;

	Sprite& addToManager();

	Sprite& removeFromManager();

	Sprite& loadSpriteImage(std::string fileLocation);

	Sprite& setFrameSize(int width, int height);

	Sprite& setPosition(double x, double y);

	std::pair<double, double> getPosition();

	Sprite& setSize(double width, double height);

	std::pair<double, double> getSize();

	Sprite& setAlpha(Uint8 a);

	Sprite& pushFrameRow(std::string animationID, int startX, int startY, int spaceX, int spaceY, int numFrames);

	Sprite& pushFrame(std::string animationID, int x, int y);

	Sprite& setAnimation(std::string animationID);

	Sprite& setAnimationRaw(std::string animationID);

	std::string getCurrentAnimation();

	int getCurrentAnimationSize();

	Sprite& setFrame(int frame);

	int getFrame();

	Sprite& setAngle(double ang);

	Sprite& setCenter(int x, int y);

	SDL_Point getCenter();

	Sprite& setDelay(unsigned int time);

	Sprite& resetTicks();

	Sprite& setCollide(bool col);

	bool getCollide();

	Sprite& nextFrame();

	Sprite& setEnabled(bool e);
	bool getEnabled();

	bool finishedAnimation();

	bool finishedFrame();

	Sprite& draw();

	SDL_Rect getBox();

	Sprite getSprite();

	Sprite& getSpriteReference();

	Sprite& setSprite(Sprite s);

	Sprite& setCamera(SDL_Rect *cam = NULL);
	SDL_Rect * getCamera();

	SDL_Texture* getTexture();

	Sprite& setVisible(bool vis);

	bool getVisible();

	Sprite & setName(std::string n);

	std::string getName();

private:
	SDL_Rect * camera = NULL;
	std::unordered_map<std::string, Texture> images;
	std::string spriteName = "";

	// a list of animations (only need to store 2 ints, x/y, because width/height are always the same)
	std::unordered_map<std::string, std::vector<std::pair<int, int>>> animations;

	// Variables relating to the Sprite
	std::pair<int, int> frameSize;
	std::pair<double, double> position;
	std::pair<double, double> scale;
	std::pair<double, double> scaleSize;
	std::string currentAnimation;
	SDL_Point center;
	SDL_Rect box;
	int currentFrame;
	int delay, ticks;
	double angle;
	bool collide, visible, inManager, enabled;

	// This was the file location of the image we loaded
	std::string location;
};

bool checkCollision(SDL_Rect *, SDL_Rect *);

bool checkCollision(Circle &, SDL_Rect &);

bool isEqualBox(SDL_Rect *, SDL_Rect *);

#endif // SPRITE_H
