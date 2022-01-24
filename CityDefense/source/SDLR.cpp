#include "../include/SDLR.h"

SDLR::SDLR()
{
	//Initialize SDL, SDL_image, SDL_ttf, SDL_mixer
	if (SDL_Init(SDL_INIT_VIDEO) == 0)
		cout << "Initialized SDL\n";
	if (IMG_Init(IMG_INIT_PNG) > 0)
		cout << "Initialized IMG\n";
	if (TTF_Init() == 0)
		cout << "Initialized TTF\n";

	//Set display hint and render draw color
	if (SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		cout << "Set Hint Quality to Linear\n";
	if (SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "1"))
		cout << "Enabled OpenGL Shaders\n";
}
