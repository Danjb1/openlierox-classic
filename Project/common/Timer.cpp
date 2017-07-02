/////////////////////////////////////////
//
//   Auxiliary Software class library
//
/////////////////////////////////////////


// Timer
// Created 12/11/01
// Jason Boettcher


#include "defs.h"

int		Frames = 0;
float	OldFPSTime = 0;
int		Fps = 0;

///////////////////
// Get the number of milliseconds since SDL started the timer
float GetMilliSeconds(void)
{
	return (float)SDL_GetTicks() * 0.001f;
}


///////////////////
// Get the frames per second count
int GetFPS(void)
{
	Frames++;

	if(GetMilliSeconds() - OldFPSTime >= 1.0f) {
		OldFPSTime = GetMilliSeconds();
		Fps = Frames;
		Frames = 0;
	}

	return Fps;
}