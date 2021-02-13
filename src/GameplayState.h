#pragma once

#include <Player.h>
#include <Ball.h>

struct GameplayState
{
	int frameNumber;
	Player players[2];
	Ball ball;

	void reset();
	void update(int _input[2]);
};
