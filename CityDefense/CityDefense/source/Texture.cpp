#include "..\include\Texture.h"

Texture::Texture()
{
	gRenderer = SDLR::gRenderer;
	scale = { 1, 1 };
}

Texture::~Texture()
{
	//free();
}

bool Texture::loadImage(string path)
{
	//Get rid of preexisting texture
	free();

	//Load path into surface
	SDL_Surface *loadFile = IMG_Load(path.c_str());

	//Print out if the path couldn't be loaded
	if (!loadFile)
	{
		cout << "Path: " << path << ", not found!\n";
		return false;
	}

	//Create texture from surface pixels
	texture = SDL_CreateTextureFromSurface(gRenderer, loadFile);

	//Get image dimensions
	width = loadFile->w;
	height = loadFile->h;
	scaleWidth = width;
	scaleHeight = height;

	//Get rid of old surface
	SDL_FreeSurface(loadFile);

	//Return success
	return texture != NULL;
}

bool Texture::loadFont(std::string textureText, SDL_Color textColor, TTF_Font * gFont, int wrapLength)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(gFont, textureText.c_str(), textColor, wrapLength);

	//Create texture from surface pixels
	texture = SDL_CreateTextureFromSurface(gRenderer, textSurface);

	//Get image dimensions
	width = textSurface->w;
	height = textSurface->h;
	scaleWidth = width;
	scaleHeight = height;

	//Get rid of old surface
	SDL_FreeSurface(textSurface);

	//Return success
	return texture != NULL;
}

void Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, (int)width, (int)height };

	//Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	renderQuad.w = (int)(renderQuad.w * scale.first);
	renderQuad.h = (int)(renderQuad.h * scale.second);

	SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, flip);
}

void Texture::free()
{
	//Remove texture if it exists
	if (texture != NULL) {
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

void Texture::setSize(double w, double h)
{
	scaleWidth = w;
	scaleHeight = h;
	scale.first = scaleWidth / width;
	scale.second = scaleHeight / height;
}

void Texture::setScale(double w, double h)
{
	scale = { w, h };
}

void Texture::setAlpha(Uint8 a)
{
	SDL_SetTextureAlphaMod(texture, a);
}
