#include "..\include\Button.h"

Button::Button()
{
	gRenderer = SDLR::gRenderer;
	
	mousePos = &SDLR::mousePosition;
	mouseButton = &SDLR::mouseButton;

	clip = { 0, 0, 200, 100 };

	Button::buttonManager.push_back(this);
}

Button::~Button()
{
	Button::buttonManager.erase(remove(Button::buttonManager.begin(), Button::buttonManager.end(), this), Button::buttonManager.end());
}

Button& Button::setText(string t, SDL_Color col, TTF_Font* font)
{
	text = t;
	if (text != "")
	{
		if (font)
			lastFont = font;
		if (lastFont != NULL) {
			lastColor = col;
			name.loadFont(text, lastColor, lastFont, (int)width);
			bx = (width / 2.0) - (name.scaleWidth / 2.0);
			by = (height / 2.0) - (name.scaleHeight / 2.0);
		}
	}
	return *this;
}

Button& Button::setSize(double w, double h)
{
	width = w;
	height = h;
	image.setSize(w, h*3);
	setText(text);
	return *this;
}

Button& Button::setPosition(double xpos, double ypos)
{
	x = xpos;
	y = ypos;
	return *this;
}

Button & Button::setImage(string path)
{
	image.loadImage(path.c_str());
	return *this;
}

Button & Button::setImage(Texture img)
{
	image = img;
	return *this;
}

Button & Button::setCamera(SDL_Rect * cam)
{
	camera = cam;
	return *this;
}

void Button::setVisible(bool v)
{
	visible = v;
	refresh();
}

bool Button::getVisible()
{
	return visible;
}

bool Button::Pressed()
{
	if (released && visible) {
		released = false;
		return true;
	}
	return false;
}

void Button::updateText(string t)
{
	text = t;
	if (text != "")
	{
		if (lastFont == NULL)
			return;
		name.loadFont(text, lastColor, lastFont, (int)width);
		bx = (width / 2.0) - (name.scaleWidth / 2.0);
		by = (height / 2.0) - (name.scaleHeight / 2.0);
	}
}

void Button::handleEvents(SDL_Event * e)
{
	if (visible && (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP))
	{
		int cx = (camera == NULL) ? 0 : camera->x;
		int cy = (camera == NULL) ? 0 : camera->y;
		if (mousePos->first >= x - cx && mousePos->second >= y - cy && mousePos->first <= x + width - cx && mousePos->second <= y + height - cy)
		{
			if (!pressed)
				clip.y = 100;

			if (!pressed && e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT)
			{
				pressed = true;
				clip.y = 200;
			}
			else if (pressed && e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT)
			{
				pressed = false;
				released = true;
				clip.y = 100;
			}
		}
		else
		{
			clip.y = 0;
			pressed = false;
		}
	}
}

void Button::render(int nx, int ny)
{
	if (visible == true) {
		if (nx > 0)
			x = nx;
		if (ny > 0)
			y = ny;
		if (camera == NULL) {
			image.render((int)x, (int)y, &clip);
			name.render((int)(bx + x), (int)(by + y));
		}
		else if (camera != NULL) {
			image.render((int)x - camera->x, (int)y - camera->y, &clip);
			name.render((int)(bx + x) - camera->x, (int)(by + y) - camera->y);
		}
	}
}

void Button::refresh()
{
	released = false;
	clip.y = 0;
}
