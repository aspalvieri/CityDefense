#ifndef TIMER_H
#define TIMER_H

#include "Includes.h"

class Timer
{
public:
	Timer();

	void start();
	void stop();
	void reset();

	Uint32 getTicks();

private:
	Uint32 ticks;
	bool started;
};

#endif //TIMER_H 