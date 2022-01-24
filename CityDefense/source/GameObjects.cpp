#include "../include/Game.h"

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

	farmSprite.loadSpriteImage("bin/images/smallhouse.png")
		.setCamera(camera)
		.setFrameSize(32, 32)
		.setSize(TILE_SIZE, TILE_SIZE)
		.setDelay(12)
		.pushFrameRow("Idle", 0, 0, 32, 0, 8)
		.pushFrameRow("Stopped", 0, 32, 32, 0, 1)
		.setAnimation("Idle");

	basicTowerSprite.loadSpriteImage("bin/images/basictower.png")
		.setCamera(camera)
		.setFrameSize(32, 32)
		.setSize(TILE_SIZE, TILE_SIZE)
		.pushFrame("Idle", 0, 0)
		.setAnimation("Idle");

	tilesForest.loadImage("bin/images/tilesheetforest.png");
	canPlace.loadImage("bin/images/greenpixel.png");
	cantPlace.loadImage("bin/images/redpixel.png");
	canPlacePoor.loadImage("bin/images/yellowpixel.png");
	gridline.loadImage("bin/images/gridline.png");

	uipanel.loadImage("bin/images/uipanel.png");
	uibg.loadImage("bin/images/brownpixel.png");
		uibg.setSize(BUTTON_SIZE, BUTTON_SIZE);
	tileHighlight.loadImage("bin/images/highlight.png");
	buttonHighlight.loadImage("bin/images/highlightred.png");
		buttonHighlight.setSize(BUTTON_SIZE, BUTTON_SIZE);
}

void Game::buildObjects()
{
	powerPlant.self = powerPlantSprite;
	powerPlant.setCost("Power Plant", 5, 20, 10)
		.setIncome(5, 0)
		.setButton(tileButton, &tileHighlight)
		.setStorage(0, 25, 0);
	addButtonObject(&powerPlantSprite, &powerPlant);

	reactor.self = reactorSprite;
	reactor.setCost("Reactor", 5, 25, 0)
		.setIncome(0, 3)
		.setButton(tileButton, &tileHighlight)
		.setStorage(0, 0, 25);
	addButtonObject(&reactorSprite, &reactor);

	smallFarm.self = farmSprite;
	smallFarm.setCost("Small Farm", 0, 40, 0)
		.setIncome(-1, -1)
		.setButton(tileButton, &tileHighlight)
		.setStorage(4, 0, 0);
	addButtonObject(&farmSprite, &smallFarm);// , TILE_DIRT);

	basicTower.self = basicTowerSprite;
	basicTower.setCost("Basic Tower", 1, 10, 25)
		.setIncome(-1, -1)
		.setButton(tileButton, &tileHighlight);
	addButtonObject(&basicTowerSprite, &basicTower);
}
