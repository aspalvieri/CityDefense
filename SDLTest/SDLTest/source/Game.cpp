#include "..\include\Game.h"

bool Game::runGame()
{
	while (!quit)
	{
		SDL_RenderClear(gRenderer);
		
		while (SDL_PollEvent(&e) != 0)
		{
			a.handleEvents(&e);
			b.handleEvents(&e);

			//Update the user's mouse properties
			if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
				*mouseButton = SDL_GetMouseState(&mousePos->first, &mousePos->second);

			//User presses exit
			if (e.type == SDL_QUIT)
				quit = true;
		}

		a.render();
		b.render();
		if (a.Pressed()) {
			b.setVisible(true);
			a.setVisible(false);
		}
		if (b.Pressed()) {
			a.setVisible(true);
			b.setVisible(false);
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
		.setText("S-A", { 255,255,0 }, fontManager[18]);
	b.setImage("bin/images/button.png")
		.setPosition(350, 250)
		.setSize(50, 50)
		.setText("S-B", { 0,255,255 }, fontManager[18])
		.setVisible(false);
}

void Game::buildImages()
{
}

