#include "../include/Game.h"

void Game::handleEvents()
{
	while (SDL_PollEvent(&e) != 0)
	{
		//Update the user's mouse properties
		if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
			if ((e.type == SDL_MOUSEBUTTONUP) && mousePos->first < UI_PLACE_X)
				targetObject = NULL;
			*mouseButton = SDL_GetMouseState(&mousePos->first, &mousePos->second);
			SDLR::mPos.x = mousePos->first;
			SDLR::mPos.y = mousePos->second;
			if (mousePos->first >= UI_PLACE_X)
				hideObject = true;
			else
				hideObject = false;
		}

		//Handle events for all buttons
		for (auto & button : Button::buttonManager) {
			if (button->getObjectButton()) {
				if (!currentObject) {
					if (mousePos->first >= UI_PLACE_X)
						button->forceRefresh();
					else
						button->handleEvents(&e);
				}
			}
			else {
				button->handleEvents(&e);
			}
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
					if (currentObject->requiredTile != emptyTile && checkCollision(&currentObject->self.getBox(), &tiles[x][y]->getBox()) && currentObject->requiredTile != tiles[x][y]->getClip()) {
						currentObject->canPlace = false;
					}
				}
			}
			for (auto & object : objects) {
				if (object->getCollide() && checkCollision(camera, &object->self.getBox()) && checkCollision(&currentObject->self.getBox(), &object->self.getBox())) 
				{
					if (currentObject->requiredType != "" && object->type == currentObject->requiredType && isEqualBox(&object->self.getBox(), &currentObject->self.getBox())) {
						currentObject->canPlace = true;
						continue;
					}
					currentObject->canPlace = false;
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !hideObject && currentObject->canPlace && currentObject->hasCost(&population, &populationMax, &gold, &stone)) {
				currentObject->removeCost(&population, &populationMax, &gold, &goldStorage, &stone, &stoneStorage, &goldIncome, &stoneIncome);
				objects.push_back(new Object(*currentObject));
				objects.back()->self.setFrame(currentObject->self.getFrame());
				objects.back()->tileButton.setVisible(true);
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

		//Handle events for buttonObjects
		for (auto & butob : buttonObjects) {
			if (!currentObject && get<0>(butob)->Pressed()) {
				if (targetObject)
					targetObject = NULL;
				delete currentObject;
				currentObject = new Object(*get<2>(butob), false);
				currentObject->self.setCamera(camera);
				canPlace.setScale(currentObject->self.getSize().first, currentObject->self.getSize().second);
				canPlacePoor.setScale(currentObject->self.getSize().first, currentObject->self.getSize().second);
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

		if (deselectTarget.Pressed()) {
			targetObject = NULL;
			deselectTarget.setVisible(false);
		}

		if (mapgenButton.Pressed()) {
			generateMap(&tilesForest);
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
	capFpsTimer.start();

	//Income every second
	if (incomeFrames++ >= 60) {
		incomeFrames = 0;
		gold += goldIncome;
		stone += stoneIncome;
	}

	for (auto & sprite : Sprite::spriteManager) {
		sprite->nextFrame();
	}

	if (currentObject)
		currentObject->self.nextFrame();

	//Check objects for updates
	for (auto & object : objects) {
		if (object != currentObject) {
			if (object->tileButton.Pressed()) {
				targetObject = object;
				if (targetObject == preTargetObject)
					targetObject = NULL;
			}
			if (object->setToDelete) {
				object->self.removeFromManager();
				objects.erase(remove(objects.begin(), objects.end(), object), objects.end());
			}
		}
	}

	//Check storages
	if (gold > goldStorage)
		gold = goldStorage;
	else if (gold < 0)
		gold = 0;
	if (stone > stoneStorage)
		stone = stoneStorage;
	else if (stone < 0)
		stone = 0;

	//Update stone display
	if (preStone != stone || preStoneStorage != stoneStorage || preStoneIncome != stoneIncome) {
		preStone = stone;
		preStoneStorage = stoneStorage;
		preStoneIncome = stoneIncome;
		preAmp = (stoneIncome >= 0) ? "+" : "";
		if (stone >= stoneStorage)
			stoneText.loadFont("Stone: " + to_string(stone) + "/" + to_string(stoneStorage) + " (" + preAmp + to_string(stoneIncome) + ")", { 255,0,0 }, fontManager[18], UI_X);
		else
			stoneText.loadFont("Stone: " + to_string(stone) + "/" + to_string(stoneStorage) + " (" + preAmp + to_string(stoneIncome) + ")", { 255,255,255 }, fontManager[18], UI_X);
		if (currentObject) //Forces update on selection target
			preCurrentObject = NULL;
	}
	//Update gold display
	if (preGold != gold || preGoldStorage != goldStorage || preGoldIncome != goldIncome) {
		preGold = gold;
		preGoldStorage = goldStorage;
		preGoldIncome = goldIncome;
		preAmp = (goldIncome >= 0) ? "+" : "";
		if (gold >= goldStorage)
			goldText.loadFont("Gold: " + to_string(gold) + "/" + to_string(goldStorage) + " (" + preAmp + to_string(goldIncome) + ")", { 255,0,0 }, fontManager[18], UI_X);
		else
			goldText.loadFont("Gold: " + to_string(gold) + "/" + to_string(goldStorage) + " (" + preAmp + to_string(goldIncome) + ")", { 255,255,255 }, fontManager[18], UI_X);
		if (currentObject) //Forces update on selection target
			preCurrentObject = NULL;
	}
	//Update population display
	if (prePop != population || prePopMax != populationMax) {
		prePop = population;
		prePopMax = populationMax;
		if (population >= populationMax)
			populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,0,0 }, fontManager[18], UI_X);
		else
			populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,255,255 }, fontManager[18], UI_X);
		if (currentObject) //Forces update on selection target
			preCurrentObject = NULL;
	}

	//Update target
	if (preTargetObject != targetObject) {
		preTargetObject = targetObject;
		if (targetObject) {
			tText = stringstream(); //Clears and empties the given stringstream
			tText << targetObject->name << "\n";
			tText << "Population: " << targetObject->population << "\n\n";
			tText << "Information:\n";
			if (targetObject->goldIncome != 0)
				tText << "Gold Income " << ((targetObject->goldIncome >= 0) ? "+" : "") << targetObject->goldIncome << "\n";
			if (targetObject->stoneIncome != 0)
				tText << "Stone Income " << ((targetObject->stoneIncome >= 0) ? "+" : "") << targetObject->stoneIncome << "\n";
			if (targetObject->populationMax > 0)
				tText << "Max Population +" << targetObject->populationMax << "\n";
			if (targetObject->goldStorage > 0)
				tText << "Gold Storage +" << targetObject->goldStorage << "\n";
			if (targetObject->stoneStorage > 0)
				tText << "Stone Storage +" << targetObject->stoneStorage << "\n";
			targetText.loadFont(tText.str(), { 255,255,255 }, fontManager[18], UI_X);
		}
	}

	//Update building information
	if (preCurrentObject != currentObject) {
		preCurrentObject = currentObject;
		if (currentObject) {
			tText = stringstream();
			tText << currentObject->name << "\n\n\nPrice";
			currentObjectTextName.loadFont(tText.str(), { 255,255,255 }, fontManager[18], UI_X - 56);
			tText = stringstream();
			tText << "Population: " << currentObject->population;
			currentObjectTextPopulationCost.loadFont(tText.str(), (population + currentObject->population <= populationMax) ? COLOR_WHITE : COLOR_RED, fontManager[18], UI_X - 56);
			tText = stringstream();
			tText << "Gold: " << currentObject->goldCost;
			currentObjectTextGoldCost.loadFont(tText.str(), (gold - currentObject->goldCost >= 0) ? COLOR_WHITE : COLOR_RED, fontManager[18], UI_X - 56);
			tText = stringstream();
			tText << "Stone: " << currentObject->stoneCost;
			currentObjectTextStoneCost.loadFont(tText.str(), (stone - currentObject->stoneCost >= 0) ? COLOR_WHITE : COLOR_RED, fontManager[18], UI_X - 56);
			tText = stringstream();
			tText << "Information:\n";
			if (currentObject->goldIncome != 0)
				tText << "Gold Income " << ((currentObject->goldIncome >= 0) ? "+" : "") << currentObject->goldIncome << "\n";
			if (currentObject->stoneIncome != 0)
				tText << "Stone Income " << ((currentObject->stoneIncome >= 0) ? "+" : "") << currentObject->stoneIncome << "\n";
			if (currentObject->populationMax > 0)
				tText << "Max Population +" << currentObject->populationMax << "\n";
			if (currentObject->goldStorage > 0)
				tText << "Gold Storage +" << currentObject->goldStorage << "\n";
			if (currentObject->stoneStorage > 0)
				tText << "Stone Storage +" << currentObject->stoneStorage << "\n";
			currentObjectTextInfo.loadFont(tText.str(), { 255,255,255 }, fontManager[18], UI_X - 56);
		}
	}

	//Update button objects
	for (auto & butob : buttonObjects) {
		if ((currentObject || targetObject) && get<0>(butob)->getVisible())
			get<0>(butob)->setVisible(false);
		else if ((!currentObject && !targetObject) && !get<0>(butob)->getVisible())
			get<0>(butob)->setVisible(true);
	}

	//Update cancel button
	if ((!currentObject || targetObject) && cancelButton.getVisible())
		cancelButton.setVisible(false);
	else if ((currentObject && !targetObject) && !cancelButton.getVisible())
		cancelButton.setVisible(true);

	//Update deselect button
	if (targetObject && !deselectTarget.getVisible())
		deselectTarget.setVisible(true);
	else if (!targetObject && deselectTarget.getVisible())
		deselectTarget.setVisible(false);

	//Update FPS text
	averageFPS = countedFrames / (fpsTimer.getTicks() / 1000.f);
	tText = stringstream();
	tText << "FPS: " << std::setprecision(2) << std::fixed << averageFPS;
	fpsText.loadFont(tText.str(), COLOR_WHITE, fontManager[18], UI_X);
	countedFrames++;

	//Cap FPS
	if (averageFPS > (SCREEN_FPS + 1)) {
		int frameTicks = capFpsTimer.getTicks();
		if (frameTicks < SCREEN_TICK_PER_FRAME) {
			SDL_Delay(SCREEN_TICK_PER_FRAME - frameTicks);
		}
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
		if (checkCollision(&sprite->getBox(), camera))
			sprite->draw();
	}

	//Draw buttons for tiles
	for (auto & object : objects) {
		object->tileButton.render();
		if (targetObject == object)
			object->highlight->render(object->self.getBox().x - camera->x, object->self.getBox().y - camera->y);
	}

	//Make a class for this, or functions
	//Renders placement object
	if (currentObject && !hideObject) {
		currentObject->self.draw();
		if (currentObject->canPlace) {
			if (currentObject->hasCost(&population, &populationMax, &gold, &stone))
				canPlace.render(currentObject->self.getBox().x - camera->x, currentObject->self.getBox().y - camera->y);
			else
				canPlacePoor.render(currentObject->self.getBox().x - camera->x, currentObject->self.getBox().y - camera->y);
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
	for (auto & butob : buttonObjects) {
		if (!currentObject && !targetObject) {
			uibg.render(get<1>(butob).getBox().x, get<1>(butob).getBox().y);
			get<1>(butob).draw();
			get<0>(butob)->render();
			if (!get<2>(butob)->hasCost(&population, &populationMax, &gold, &stone)) {
				buttonHighlight.render(get<1>(butob).getBox().x, get<1>(butob).getBox().y);
			}
		}
	}
	cancelButton.render();
	deselectTarget.render();
	mapgenButton.render();
	populationText.render(UI_PLACE_X + 32, UI_PLACE_Y + 30);
	goldText.render(UI_PLACE_X + 32, UI_PLACE_Y + 50);
	stoneText.render(UI_PLACE_X + 32, UI_PLACE_Y + 70);
	fpsText.render(UI_PLACE_X + 32, UI_PLACE_Y + 500);
	if (targetObject && !currentObject)
		targetText.render(UI_PLACE_X + 32, UI_PLACE_Y + 140);
	if (currentObject && !targetObject)
	{
		currentObjectTextName.render(UI_PLACE_X + 32, UI_PLACE_Y + 140);
		currentObjectTextPopulationCost.render(UI_PLACE_X + 32, UI_PLACE_Y + 160);
		currentObjectTextGoldCost.render(UI_PLACE_X + 32, UI_PLACE_Y + 216);
		currentObjectTextStoneCost.render(UI_PLACE_X + 32, UI_PLACE_Y + 234);
		currentObjectTextInfo.render(UI_PLACE_X + 32, UI_PLACE_Y + 270);
	}
	
	SDL_RenderPresent(gRenderer);
}

void Game::initialize()
{
	buildFontManager();
	buildButtons();
	buildImages();
	buildTextDisplays();
	buildObjects();

	generateMap(&tilesForest);
	
	setCamera();
	fpsTimer.start();
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

void Game::buildTextDisplays()
{
	//Initial text loads
	stoneText.loadFont("Stone: " + to_string(stone) + "/" + to_string(stoneStorage) + " (+0)", { 255,255,255 }, fontManager[18], UI_X);
	goldText.loadFont("Gold: " + to_string(gold) + "/" + to_string(goldStorage) + " (+0)", { 255,255,255 }, fontManager[18], UI_X);
	populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,255,255 }, fontManager[18], UI_X);
}

//Frames to Seconds
int Game::FTS(double time)
{
	return (int)(round(time * SCREEN_FPS));
}

void Game::addButtonObject(Sprite *spr, Object *obj)
{
	buttonObjects.push_back({ new Button(uiButton), *spr, new Object(*obj) });
	get<2>(buttonObjects.back())->self.removeFromManager();
	get<1>(buttonObjects.back()).setPosition(SCREEN_WIDTH - UI_X + bobx, boby)
		.setSize(BUTTON_SIZE, BUTTON_SIZE)
		.setCamera(NULL)
		.removeFromManager();
	get<0>(buttonObjects.back())->setPosition(get<1>(buttonObjects.back()).getPosition().first, get<1>(buttonObjects.back()).getPosition().second)
		.addToManager();
	bobx += 68;
	if (bobx > 168)
	{
		boby += 60;
		bobx = 32;
	}
}

void Game::addButtonObject(Sprite *spr, Object *obj, SDL_Rect req)
{
	addButtonObject(spr, obj);
	get<2>(buttonObjects.back())->requiredTile.setClip(req);
}
