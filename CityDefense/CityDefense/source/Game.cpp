#include "..\include\Game.h"

void Game::handleEvents()
{
	while (SDL_PollEvent(&e) != 0)
	{
		//Update the user's mouse properties
		if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
			*mouseButton = SDL_GetMouseState(&mousePos->first, &mousePos->second);
			if (mousePos->first >= SCREEN_WIDTH - UI_X)
				hideObject = true;
			else
				hideObject = false;
		}

		//Move this autism
		if (currentObject) {
			currentObject->moveToMouse(mousePos->first + camera->x, mousePos->second + camera->y);
			currentObject->canPlace = currentObject->defaultPlace;

			for (int y = maxCamY; y <= maxCamH; y++) {
				for (int x = maxCamX; x <= maxCamW; x++) {
					if (tiles[x][y]->getCollide() && checkCollision(&currentObject->self.getBox(), &tiles[x][y]->getBox())) {
						currentObject->canPlace = false;
					}
				}
			}
			for (auto & object : objects) {
				if (object->getCollide() && checkCollision(camera, &object->self.getBox())
					&& checkCollision(&currentObject->self.getBox(), &object->self.getBox())) 
				{
					if (currentObject->requiredType != "" && object->type == currentObject->requiredType && isEqualBox(&object->self.getBox(), &currentObject->self.getBox())) {
						currentObject->canPlace = true;
						continue;
					}
					currentObject->canPlace = false;
				}
			}
			if (!hideObject && currentObject->canPlace && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
				objects.push_back(new Object(*currentObject));
				objects.back()->self.setFrame(currentObject->self.getFrame());
				if (!multiPlace) {
					placedOnce = false;
					delete currentObject;
					currentObject = NULL;
					for (auto & butob : buttonObjects) {
						get<0>(butob)->setVisible(true);
					}
					cancelButton.setVisible(false);
				}
				else if (multiPlace) {
					placedOnce = true;
					currentObject->self.setFrame(0);
				}
			}
		}

		//Handle events for camera
		cameraEvents(&e);

		//Handle events for all buttons
		for (auto & button : Button::buttonManager) {
			button->handleEvents(&e);
		}

		//Handle events for buttonObjects
		for (auto & butob : buttonObjects) {
			if (!currentObject && get<0>(butob)->Pressed()) {
				delete currentObject;
				currentObject = new Object(*get<2>(butob), false);
				currentObject->self.setCamera(camera);
				canPlace.setScale(currentObject->self.getSize().first, currentObject->self.getSize().second);
				cantPlace.setScale(currentObject->self.getSize().first, currentObject->self.getSize().second);
				for (auto & butob2 : buttonObjects) {
					get<0>(butob)->setVisible(false);
				}
				cancelButton.setVisible(true);
			}
		}

		if (cancelButton.Pressed()) {
			delete currentObject;
			currentObject = NULL;
			for (auto & butob : buttonObjects) {
				get<0>(butob)->setVisible(true);
			}
			cancelButton.setVisible(false);
		}

		if (mapgenButton.Pressed()) {
			mapfunc->generateMap(&tilesForest);
		}

		//User presses a key down
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				multiPlace = true;
				break;
			case SDLK_ESCAPE:
				quit = true;
				break;
			default:
				break;
			}
		}
		if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				multiPlace = false;
				if (placedOnce) {
					placedOnce = false;
					delete currentObject;
					currentObject = NULL;
					for (auto & butob : buttonObjects) {
						get<0>(butob)->setVisible(true);
					}
					cancelButton.setVisible(false);
				}
				break;
			default:
				break;
			}
		}

		//User presses exit
		if (e.type == SDL_QUIT)
			quit = true;
	}
}

void Game::update()
{
	for (auto & sprite : Sprite::spriteManager) {
		sprite->nextFrame();
	}

	if (currentObject)
		currentObject->self.nextFrame();

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

	//Make a class for this, or functions
	if (currentObject && !hideObject) {
		currentObject->self.draw();
		if (currentObject->canPlace) {
			canPlace.render(currentObject->self.getBox().x - camera->x, currentObject->self.getBox().y - camera->y);
		}
		if (!currentObject->canPlace) {
			cantPlace.render(currentObject->self.getBox().x - camera->x, currentObject->self.getBox().y - camera->y);
		}

		//Render gridlines
		for (int y = maxCamY; y <= maxCamH; y++) {
			for (int x = maxCamX; x <= maxCamW; x++) {
				//gridline.render(tiles[x][y]->getBox().x - camera->x, tiles[x][y]->getBox().y - camera->y);
			}
		}
	}

	//Draw UI Panel
	uipanel.render(SCREEN_WIDTH - UI_X, 0);
	if (!currentObject) {
		for (auto & butob : buttonObjects) {
			uibg.render(get<1>(butob).getPosition().first, get<1>(butob).getPosition().second);
			get<1>(butob).draw();
			get<0>(butob)->render();
		}
	}
	cancelButton.render();
	mapgenButton.render();
	
	SDL_RenderPresent(gRenderer);
}

void Game::initialize()
{
	buildFontManager();
	buildButtons();
	buildImages();
	buildObjects();

	mapfunc = new MapFunc(this);
	mapfunc->generateMap(&tilesForest);
	
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
	posx = camera->x;
	posy = camera->y;
}

Game::~Game()
{
}

void Game::cameraEvents(SDL_Event * e)
{
	//User presses a key down
	if (e->type == SDL_KEYDOWN) {
		switch (e->key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w:
			up = true;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			down = true;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			right = true;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			left = true;
			break;
		default:
			break;
		}
	}

	//User presses a key up
	if (e->type == SDL_KEYUP) {
		switch (e->key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w:
			up = false;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			down = false;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			right = false;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			left = false;
			break;
		default:
			break;
		}
	}
}

void Game::moveCamera()
{
	if (up || down || left || right) {
		if (currentObject)
			currentObject->moveToMouse(mousePos->first + camera->x, mousePos->second + camera->y);
		xspeed = (right - left) * camSpeed;
		yspeed = (down - up) * camSpeed;
		if (xspeed != 0 && yspeed != 0) {
			xspeed /= sqrt(2.0);
			yspeed /= sqrt(2.0);
		}
		posx += xspeed;
		posy += yspeed;
		if (posx < 0) posx = 0;
		if (posy < 0) posy = 0;
		if (posx > (MAP_X * TILE_SIZE) - camera->w) posx = (MAP_X * TILE_SIZE) - camera->w;
		if (posy > (MAP_Y * TILE_SIZE) - camera->h) posy = (MAP_Y * TILE_SIZE) - camera->h;
		camera->x = (int)posx;
		camera->y = (int)posy;
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

int Game::randomInt(int min, int max)
{
	return uniform_int_distribution<int>{min, max}(rng);
}

double Game::randomDouble(double min, double max)
{
	return uniform_real_distribution<double>{min, max}(rng);
}

bool Game::running()
{
	return !quit;
}

void Game::destroy()
{
	//Destroy window
	SDL_DestroyRenderer(SDLR::gRenderer);
	SDL_DestroyWindow(SDLR::gWindow);
	SDLR::gWindow = NULL;
	SDLR::gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	//Free sprites in the sprite manager
	for (auto & sprite : Sprite::spriteManager) {
		sprite->freeImageSet();
	}

	//Free textures in button manager
	for (auto & button : Button::buttonManager) {
		button->getImage().free();
	}

	//Free tileset
	tilesForest.free();
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
	uiButton.setImage("bin/images/uibutton.png")
		.setSize(BUTTON_SIZE, BUTTON_SIZE);

	cancelButton.setImage("bin/images/button.png")
		.addToManager()
		.setVisible(false)
		.setSize(128, 32)
		.setPosition(SCREEN_WIDTH - UI_X + 65, 50)
		.setText("Cancel", { 255,255,255 }, fontManager[18]);

	mapgenButton.setImage("bin/images/button.png")
		.addToManager()
		.setSize(160, 32)
		.setPosition(SCREEN_WIDTH - UI_X + 48, 650)
		.setText("Generate Map", { 255,255,255 }, fontManager[18]);
}

void Game::buildImages()
{
	powerPlantSprite.loadSpriteImage("bin/images/powerplant.png")
		.setCamera(camera)
		.setFrameSize(100, 100)
		.setSize(TILE_SIZE * 2, TILE_SIZE * 2)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 100, 0, 7)
		.pushFrameRow("Idle", 0, 100, 100, 0, 7)
		.setAnimation("Idle");

	reactorSprite.loadSpriteImage("bin/images/reactor.png")
		.setCamera(camera)
		.setFrameSize(100, 100)
		.setSize(TILE_SIZE * 2, TILE_SIZE * 2)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 100, 0, 6)
		.setAnimation("Idle");

	rockSprite.loadSpriteImage("bin/images/rock.png")
		.setCamera(camera)
		.setEnabled(false)
		.setFrameSize(TILE_SIZE, TILE_SIZE)
		.setSize(TILE_SIZE, TILE_SIZE)
		.pushFrameRow("Idle", 0, 0, TILE_SIZE, 0, 4)
		.setAnimation("Idle");

	minerSprite.loadSpriteImage("bin/images/weakminer.png")
		.setCamera(camera)
		.setFrameSize(32, 32)
		.setSize(TILE_SIZE, TILE_SIZE)
		.setDelay(6)
		.pushFrameRow("Idle", 0, 0, 32, 0, 6)
		.pushFrameRow("Idle", 0, 32, 32, 0, 6)
		.pushFrameRow("Idle", 0, 64, 32, 0, 6)
		.pushFrameRow("Idle", 0, 96, 32, 0, 6)
		.pushFrameRow("Idle", 0, 128, 32, 0, 6)
		.setAnimation("Idle");

	deleteSprite.loadSpriteImage("bin/images/delete.png")
		.setCamera(camera)
		.setFrameSize(96, 64)
		.setSize(TILE_SIZE * 3, TILE_SIZE * 1)
		.setDelay(3)
		.pushFrameRow("Idle", 0, 0, 96, 0, 2)
		.setAnimation("Idle");

	tilesForest.loadImage("bin/images/tilesheetforest.png");
	canPlace.loadImage("bin/images/greenpixel.png");
	cantPlace.loadImage("bin/images/redpixel.png");
	gridline.loadImage("bin/images/gridline.png");

	uipanel.loadImage("bin/images/uipanel.png");
	uipanel.setSize(UI_X, UI_Y);
	uibg.loadImage("bin/images/brownpixel.png");
	uibg.setSize(BUTTON_SIZE, BUTTON_SIZE);
}

void Game::buildObjects()
{
	powerPlant.self = powerPlantSprite;

	reactor.self = reactorSprite;

	deleteObject.self = deleteSprite;

	rocks.self = rockSprite;
	rocks.type = "Rock";

	rockMiner.self = minerSprite;
	rockMiner.requiredType = "Rock";
	rockMiner.defaultPlace = false;


	buttonObjects.push_back({ new Button(uiButton), powerPlantSprite, new Object(powerPlant) });
	get<2>(buttonObjects.back())->self.removeFromManager();
	get<1>(buttonObjects.back()).setPosition(SCREEN_WIDTH - UI_X + (TILE_SIZE * 1), 16 + 32)
		.setSize(BUTTON_SIZE, BUTTON_SIZE)
		.setCamera(NULL)
		.removeFromManager();
	get<0>(buttonObjects.back())->setPosition(get<1>(buttonObjects.back()).getPosition().first, get<1>(buttonObjects.back()).getPosition().second)
		.addToManager();

	buttonObjects.push_back({ new Button(uiButton), reactorSprite, new Object(reactor) });
	get<2>(buttonObjects.back())->self.removeFromManager();
	get<1>(buttonObjects.back()).setPosition(SCREEN_WIDTH - UI_X + (TILE_SIZE * 3) + 4, 16 + 32)
		.setSize(BUTTON_SIZE, BUTTON_SIZE)
		.setCamera(NULL)
		.removeFromManager();
	get<0>(buttonObjects.back())->setPosition(get<1>(buttonObjects.back()).getPosition().first, get<1>(buttonObjects.back()).getPosition().second)
		.addToManager();

	buttonObjects.push_back({ new Button(uiButton), minerSprite, new Object(rockMiner) });
	get<2>(buttonObjects.back())->self.removeFromManager();
	get<1>(buttonObjects.back()).setPosition(SCREEN_WIDTH - UI_X + (TILE_SIZE * 5) + 8, 16 + 32)
		.setSize(BUTTON_SIZE, BUTTON_SIZE)
		.setCamera(NULL)
		.removeFromManager();
	get<0>(buttonObjects.back())->setPosition(get<1>(buttonObjects.back()).getPosition().first, get<1>(buttonObjects.back()).getPosition().second)
		.addToManager();
}
