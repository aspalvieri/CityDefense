#include "../include/Game.h"

//Create sdlr class globally since constructor needs to run before static variables are built
SDLR sdlr;

//Declare static variables for Window, Renderer, and Mouse properties
SDL_Window* SDLR::gWindow = SDL_CreateWindow("City Defense", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
SDL_Renderer* SDLR::gRenderer = SDL_CreateRenderer(SDLR::gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
pair<int, int> SDLR::mousePosition = { 0, 0 };
int SDLR::mouseButton = 0;
SDL_Rect SDLR::camera = { 0, 0, SCREEN_WIDTH - UI_X, SCREEN_HEIGHT };
SDL_Rect SDLR::mPos = { 0, 0, 1, 1 };

//Declare static managers
vector<Button*> Button::buttonManager;
vector<Sprite*> Sprite::spriteManager;

int main(int argc, char* argv[])
{
	Game game;
	game.initialize();
	
	while (game.running()) {
		game.handleEvents();
		game.update();
		game.render();
	}

	game.destroy();

	return 0;
}