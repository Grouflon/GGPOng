#pragma once

#include <Player.h>
#include <Ball.h>

struct GameplayState
{
	Player p1;
	Player p2;
	Ball ball;

	void reset();
};
