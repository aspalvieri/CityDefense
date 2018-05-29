#include "..\include\Game.h"

bool Game::runGame()
{
	while (!quit)
	{
		SDL_RenderClear(gRenderer);
		
		while (SDL_PollEvent(&e) != 0)
		{
			for (auto & button : Button::buttonManager) {
				button->handleEvents(&e);
			}

			//Update the user's mouse properties
			if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
				*mouseButton = SDL_GetMouseState(&mousePos->first, &mousePos->second);

			//User presses a key down
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_ESCAPE:
					quit = true;
					break;
				default:
					break;
				}
			}

			//User presses exit
			if (e.type == SDL_QUIT)
				quit = true;
		}

		for (auto & sprite : Sprite::spriteManager) {
			sprite->nextFrame();
			sprite->draw();
		}

		a.render();
		b.render();
		if (a.Pressed()) {
			b.setVisible(true);
			a.setVisible(false);
			g.setVisible(false);
		}
		if (b.Pressed()) {
			a.setVisible(true);
			b.setVisible(false);
			g.setVisible(true);
		}

		SDL_RenderPresent(gRenderer);
	}
	return quit;
}

Game::Game()
{
	gRenderer = SDLR::gRenderer;
	gWindow = SDLR::gWindow;
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	srand((unsigned)time(0));

	mousePos = &SDLR::mousePosition;
	mouseButton = &SDLR::mouseButton;
}

Game::~Game()
{
}

void Game::initialize()
{
	buildFontManager();
	buildButtons();
	buildImages();
}

void Game::buildFontManager()
{
	//Load font sizes into fontManager for later use
	for (int i = 12; i <= 24; i+=2)
	{
		fontManager[i] = TTF_OpenFont("bin/fonts/codenewroman.ttf", i);
	}
}

void Game::buildButtons()
{
	a.setImage("bin/images/button.png")
		.setPosition(250, 250)
		.setSize(50, 50)
		.setText("Off", { 255,255,0 }, fontManager[18]);
	b.setImage("bin/images/button.png")
		.setPosition(350, 250)
		.setSize(50, 50)
		.setText("On", { 0,255,0 }, fontManager[18])
		.setVisible(false);
}

void Game::buildImages()
{
	g.loadSpriteImage("bin/images/weaponforge.png")
		.setFrameSize(100, 100)
		.setSize(100, 100)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 100, 0, 7)
		.pushFrameRow("Idle", 0, 100, 100, 0, 7)
		.setAnimation("Idle");
}

