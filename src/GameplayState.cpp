#include "GameplayState.h"	

#include <Globals.h>

void GameplayState::reset()
{
	gs.p1.position = sf::Vector2f(playerBorderOffset, gameSize.y * .5f);
	gs.p2.position = sf::Vector2f(gameSize.x - playerBorderOffset, gameSize.y * .5f);

	gs.ball.position = gameSize * .5f;
	gs.ball.velocity = ballInitialVelocity;
}

