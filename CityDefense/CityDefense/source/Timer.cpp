#include "../include/Timer.h"

Timer::Timer()
{
	ticks = 0;
	started = false;
}

void Timer::start()
{
	started = true;
	ticks = SDL_GetTicks();
}

void Timer::stop()
{
	started = false;
	ticks = 0;
}

void Timer::reset()
{
	ticks = SDL_GetTicks();
}

Uint32 Timer::getTicks()
{
	Uint32 time = 0;

	if (started == true)
		time = SDL_GetTicks() - ticks;

	return time;
}