#include "..\include\Game.h"

void Game::handleEvents()
{
	while (SDL_PollEvent(&e) != 0)
	{
		//Update the user's mouse properties
		if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
			*mouseButton = SDL_GetMouseState(&mousePos->first, &mousePos->second);

		for (auto & button : Button::buttonManager) {
			button->handleEvents(&e);
		}

		//User presses a key down
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				quit = true;
				break;
			case SDLK_w:
				up = true;
				break;
			case SDLK_s:
				down = true;
				break;
			case SDLK_d:
				right = true;
				break;
			case SDLK_a:
				left = true;
				break;
			default:
				break;
			}
		}

		//User presses a key up
		if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_w:
				up = false;
				break;
			case SDLK_s:
				down = false;
				break;
			case SDLK_d:
				right = false;
				break;
			case SDLK_a:
				left = false;
				break;
			default:
				break;
			}
		}

		//User presses exit
		if (e.type == SDL_QUIT)
			quit = true;

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
	}
}

void Game::update()
{
	for (auto & sprite : Sprite::spriteManager) {
		sprite->nextFrame();
	}

	moveCamera();
}

void Game::render() 
{
	SDL_RenderClear(gRenderer);

	//Render all tiles
	for (int y = maxCamY; y <= maxCamH; y++) {
		for (int x = maxCamX; x <= maxCamW; x++) {
			tiles[x][y]->draw();
		}
	}

	//Render all sprites
	for (auto & sprite : Sprite::spriteManager) {
		sprite->draw();
	}

	a.render();
	b.render();
	
	SDL_RenderPresent(gRenderer);
}

void Game::moveCamera()
{
	if (up || down || left || right) {
		camera->x += (right - left) * camSpeed;
		camera->y += (down - up) * camSpeed;
		if (camera->x < 0) camera->x = 0;
		if (camera->y < 0) camera->y = 0;
		if (camera->x > (MAP_X * TILE_SIZE) - camera->w) camera->x = (MAP_X * TILE_SIZE) - camera->w;
		if (camera->y > (MAP_Y * TILE_SIZE) - camera->h) camera->y = (MAP_Y * TILE_SIZE) - camera->h;
		setCamera();
		if (maxCamW == MAP_X) maxCamW = MAP_X - 1;
		if (maxCamH == MAP_Y) maxCamH = MAP_Y - 1;
	}
}

void Game::setCamera()
{
	maxCamY = (int)ceil(camera->y / TILE_SIZE);
	maxCamH = (int)(((ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE)) < MAP_Y - 1) ? ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE) + 1 : ceil(camera->y / TILE_SIZE) + ceil(camera->h / TILE_SIZE));
	maxCamX = (int)ceil(camera->x / TILE_SIZE);
	maxCamW = (int)(((ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE)) < MAP_X - 1) ? ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE) + 1 : ceil(camera->x / TILE_SIZE) + ceil(camera->w / TILE_SIZE));

}

void Game::initialize()
{
	buildFontManager();
	buildButtons();
	buildImages();

	for (int y = 0; y < MAP_Y; y++) {
		for (int x = 0; x < MAP_X; x++) {
			tiles[x][y] = new Tile(&tilesForest, TILE_GRASS, x * TILE_SIZE, y * TILE_SIZE);
		}
	}
	
	setCamera();
}

Game::Game()
{
	gRenderer = SDLR::gRenderer;
	gWindow = SDLR::gWindow;
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	srand((unsigned)time(0));

	camera = &SDLR::camera;
	mousePos = &SDLR::mousePosition;
	mouseButton = &SDLR::mouseButton;
}

Game::~Game()
{
}

bool Game::running()
{
	return !quit;
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
		.setCamera(camera)
		.setPosition(250, 250)
		.setSize(50, 50)
		.setText("Off", { 255,255,0 }, fontManager[18]);
	b.setImage("bin/images/button.png")
		.setCamera(camera)
		.setPosition(350, 250)
		.setSize(50, 50)
		.setText("On", { 0,255,0 }, fontManager[18])
		.setVisible(false);
}

void Game::buildImages()
{
	g.loadSpriteImage("bin/images/weaponforge.png")
		.setCamera(camera)
		.setFrameSize(100, 100)
		.setSize(100, 100)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 100, 0, 7)
		.pushFrameRow("Idle", 0, 100, 100, 0, 7)
		.setAnimation("Idle");

	tilesForest.loadImage("bin/images/tilesheetforest.png");
}

