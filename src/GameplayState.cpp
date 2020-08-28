#include "GameplayState.h"	

#include <Globals.h>

void GameplayState::reset()
{
	p1.position = sf::Vector2f(playerBorderOffset, gameSize.y * .5f);
	p2.position = sf::Vector2f(gameSize.x - playerBorderOffset, gameSize.y * .5f);

	ball.position = gameSize * .5f;
	ball.velocity = ballInitialVelocity;
}

