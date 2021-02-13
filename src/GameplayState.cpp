#include "GameplayState.h"	

#include <Globals.h>

void GameplayState::reset()
{
	frameNumber = 0;

	players[0].position = sf::Vector2f(playerBorderOffset, gameSize.y * .5f);
	players[1].position = sf::Vector2f(gameSize.x - playerBorderOffset, gameSize.y * .5f);

	ball.position = gameSize * .5f;
	ball.velocity = ballInitialVelocity;
}

void GameplayState::update(int _input[2])
{
	++frameNumber;

	for (int i = 0; i < 2; ++i)
	{
		players[i].update(_input[i]);
	}

	ball.update(*this);
}

