#include "..\include\Game.h"

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
						currentObject->ability.objectValue = object;
						continue;
					}
					currentObject->canPlace = false;
					if (currentObject->ability.objectValue)
						currentObject->ability.objectValue = NULL;
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT && !hideObject && currentObject->canPlace && currentObject->hasCost(&population, &populationMax, &gold, &stone)) {
				currentObject->removeCost(&population, &populationMax, &gold, &goldStorage, &stone, &stoneStorage);
				if (currentObject->ability.objectValue)
					currentObject->ability.objectValue->tileButton.removeFromManager();
				objects.push_back(new Object(*currentObject));
				objects.back()->self.setFrame(currentObject->self.getFrame());
				objects.back()->tileButton.setVisible(true);
				if (objects.back()->ability.alive)
					objects.back()->ability.enabled = true;
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
					if (currentObject->ability.objectValue)
						currentObject->ability.objectValue = NULL;
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
			mapfunc->generateMap(&tilesForest);
		}

		//User presses a key down
		if (e.type == SDL_KEYDOWN) {
			switch (e.key.keysym.sym) {
			case SDLK_LSHIFT:
			case SDLK_RSHIFT:
				multiPlace = true;
				break;
			case SDLK_r:
			{
				int grem = 0, srem = 0;
				for (auto & object : objects) {
					if (object->type == "Rock") {
						if (object->subtype == "Gold") {
							grem += object->abilityValue;
						}
						if (object->subtype == "Stone") {
							srem += object->abilityValue;
						}
					}
				}
				cout << "Gold remaining in map: " << grem << ", Stone remaining in map:" << srem << "\n";
				break;
			}
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

	for (auto & object : objects) {
		if (object != currentObject) {
			if (object->tileButton.Pressed()) {
				targetObject = object;
				if (targetObject == preTargetObject)
					targetObject = NULL;
			}
			object->runAbility();
			if (object->setToDelete) {
				object->self.removeFromManager();
				objects.erase(remove(objects.begin(), objects.end(), object), objects.end());
			}
			if (object->ability.alive && !object->ability.running) {
				object->self.setEnabled(false);
				object->ability.alive = false;
				if (object->self.hasAnimation("Stopped"))
					object->self.setAnimation("Stopped");
			}
		}
	}

	//Check storages
	if (gold > goldStorage)
		gold = goldStorage;
	if (stone > stoneStorage)
		stone = stoneStorage;

	//Update stone display
	if (preStone != stone || preStoneStorage != stoneStorage) {
		preStone = stone;
		if (stone >= stoneStorage)
			stoneText.loadFont("Stone: " + to_string(stone) + "/" + to_string(stoneStorage), { 255,0,0 }, fontManager[18], UI_X);
		else
			stoneText.loadFont("Stone: " + to_string(stone) + "/" + to_string(stoneStorage), { 255,255,255 }, fontManager[18], UI_X);
	}
	//Update gold display
	if (preGold != gold || preGoldStorage != goldStorage) {
		preGold = gold;
		if (gold >= goldStorage)
			goldText.loadFont("Gold: " + to_string(gold) + "/" + to_string(goldStorage), { 255,0,0 }, fontManager[18], UI_X);
		else
			goldText.loadFont("Gold: " + to_string(gold) + "/" + to_string(goldStorage), { 255,255,255 }, fontManager[18], UI_X);
	}
	//Update population display
	if (prePop != population || prePopMax != populationMax) {
		prePop = population;
		prePopMax = populationMax;
		if (population >= populationMax)
			populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,0,0 }, fontManager[18], UI_X);
		else
			populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,255,255 }, fontManager[18], UI_X);
	}

	//Update target
	if (preTargetObject != targetObject || (preTargetObject && preTargetObject->ability.abilityUpdated)) {
		preTargetObject = targetObject;
		if (targetObject) {
			stringstream().swap(tText); //Clears and empties the given stringstream
			tText << targetObject->name << "\n";
			if (targetObject->abilityValue)
				tText << "Remaining: " << targetObject->abilityValue << "\n";
			if (targetObject->requiredType == "Rock") {
				if (targetObject->ability.objectValue)
					tText << "Mining: " << targetObject->ability.objectValue->subtype << "\nRemaining: " << targetObject->ability.objectValue->abilityValue << "\n";
				else
					tText << "Mining: Nothing\nRemaining: 0\n";
			}
			targetText.loadFont(tText.str(), { 255,255,255 }, fontManager[18], UI_X);
		}
	}

	//Update building information
	if (preCurrentObject != currentObject) {
		preCurrentObject = currentObject;
		if (currentObject) {
			stringstream().swap(tText);
			tText << currentObject->name << "\n";
			tText << "Population: " << currentObject->population << "\n\nPrice:\nGold: " << currentObject->goldCost << "\nStone: " << currentObject->stoneCost << "\n";
			tText << "\nInformation:\n";
			if (currentObject->populationMax > 0)
				tText << "Total Population +" << currentObject->populationMax << "\n";
			if (currentObject->goldStorage > 0)
				tText << "Gold Storage +" << currentObject->goldStorage << "\n";
			if (currentObject->stoneStorage > 0)
				tText << "Stone Storage +" << currentObject->stoneStorage << "\n";
			if (currentObject->ability.alive)
				tText << currentObject->ability.printAbility() << "\n";
			currentObjectText.loadFont(tText.str(), { 255,255,255 }, fontManager[18], UI_X - 56);
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
	for (auto & butob : buttonObjects) {
		if (!currentObject && !targetObject) {
			uibg.render(get<1>(butob).getBox().x, get<1>(butob).getBox().y);
			get<1>(butob).draw();
			get<0>(butob)->render();
		}
	}
	cancelButton.render();
	deselectTarget.render();
	mapgenButton.render();
	populationText.render(UI_PLACE_X + 32, UI_PLACE_Y + 30);
	goldText.render(UI_PLACE_X + 32, UI_PLACE_Y + 50);
	stoneText.render(UI_PLACE_X + 32, UI_PLACE_Y + 70);
	if (targetObject && !currentObject)
		targetText.render(UI_PLACE_X + 32, UI_PLACE_Y + 140);
	if (currentObject && !targetObject)
		currentObjectText.render(UI_PLACE_X + 32, UI_PLACE_Y + 140);
	
	SDL_RenderPresent(gRenderer);
}

void Game::initialize()
{
	buildFontManager();
	buildButtons();
	buildImages();
	buildTextDisplays();
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
		.setPosition(SCREEN_WIDTH - UI_X + 65, 100)
		.setText("Cancel", { 255,255,255 }, fontManager[18]);

	deselectTarget.setImage("bin/images/button.png")
		.addToManager()
		.setVisible(false)
		.setSize(128, 32)
		.setPosition(SCREEN_WIDTH - UI_X + 65, 100)
		.setText("Deselect", { 255,255,255 }, fontManager[18]);

	mapgenButton.setImage("bin/images/button.png")
		.addToManager()
		.setSize(160, 32)
		.setPosition(SCREEN_WIDTH - UI_X + 48, 650)
		.setText("Generate Map", { 255,255,255 }, fontManager[18]);

	tileButton.setImage("bin/images/tilebutton.png")
		.setVisible(false)
		.setObjectButton(true)
		.setSize(TILE_SIZE, TILE_SIZE);
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

	goldRockSprite.loadSpriteImage("bin/images/goldrock.png")
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

	farmSprite.loadSpriteImage("bin/images/smallhouse.png")
		.setCamera(camera)
		.setFrameSize(32, 32)
		.setSize(TILE_SIZE, TILE_SIZE)
		.setDelay(12)
		.pushFrameRow("Idle", 0, 0, 32, 0, 8)
		.pushFrameRow("Stopped", 0, 32, 32, 0, 1)
		.setAnimation("Idle");

	tilesForest.loadImage("bin/images/tilesheetforest.png");
	canPlace.loadImage("bin/images/greenpixel.png");
	cantPlace.loadImage("bin/images/redpixel.png");
	gridline.loadImage("bin/images/gridline.png");

	uipanel.loadImage("bin/images/uipanel.png");
	uibg.loadImage("bin/images/brownpixel.png");
	uibg.setSize(BUTTON_SIZE, BUTTON_SIZE);
	tileHighlight.loadImage("bin/images/highlight.png");
}

void Game::buildTextDisplays()
{
	stoneText.loadFont("Stone: "+to_string(stone) + "/" + to_string(stoneStorage), { 255,255,255 }, fontManager[18], UI_X);
	goldText.loadFont("Gold: "+to_string(gold) + "/" + to_string(goldStorage), { 255,255,255 }, fontManager[18], UI_X);
	populationText.loadFont("Population: " + to_string(population) + "/" + to_string(populationMax), { 255,255,255 }, fontManager[18], UI_X);
}

void Game::buildObjects()
{
	powerPlant.self = powerPlantSprite;
	powerPlant.setCost("Power Plant", 5, 20, 10)
		.setButton(tileButton, &tileHighlight)
		.setStorage(0, 25, 0);

	reactor.self = reactorSprite;
	reactor.setCost("Reactor", 5, 10, 0)
		.setButton(tileButton, &tileHighlight)
		.setStorage(0, 0, 25);

	rocks.self = rockSprite;
	rocks.type = "Rock";
	rocks.subtype = "Stone";
	rocks.name = "Stone Rocks";
	rocks.setButton(tileButton, &tileHighlight);

	goldrocks.self = goldRockSprite;
	goldrocks.type = "Rock";
	goldrocks.subtype = "Gold";
	goldrocks.name = "Gold Rocks";
	goldrocks.setButton(tileButton, &tileHighlight);

	rockMiner.self = minerSprite;
	rockMiner.requiredType = "Rock";
	rockMiner.defaultPlace = false;
	rockMiner.setCost("Weak Rock Miner", 1, 25, 0)
		.setButton(tileButton, &tileHighlight);

	smallFarm.self = farmSprite;
	smallFarm.setCost("Small Farm", 0, 40, 0)
		.setButton(tileButton, &tileHighlight)
		.setStorage(3, 0, 0);

	addButtonObject(&powerPlantSprite, &powerPlant);
	addButtonObject(&reactorSprite, &reactor);
	addButtonObject(&minerSprite, &rockMiner, Ability("RockMiner", &gold, &stone, 1, FTS(1.5)) );
	addButtonObject(&farmSprite, &smallFarm, TILE_DIRT);
}

//Frames to Seconds
double Game::FTS(double time)
{
	return time * SCREEN_FPS;
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

void Game::addButtonObject(Sprite *spr, Object *obj, Ability abi)
{
	addButtonObject(spr, obj);
	get<2>(buttonObjects.back())->ability = abi;
}

void Game::addButtonObject(Sprite *spr, Object *obj, SDL_Rect req, Ability abi)
{
	addButtonObject(spr, obj, req);
	get<2>(buttonObjects.back())->ability = abi;
}
