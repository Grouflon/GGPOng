#include "Ball.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <Globals.h>

void Ball::update(GameplayState& _gs)
{
	// vertical fist
	position.y += velocity.y;
	// up border
	{
		float d = ballSize.y * .5f - position.y;
		if (d >= 0.f)
		{
			velocity.y *= -1.f;
			position.y += d;
		}
	}
	// down border
	{
		float d = (gameSize.y - ballSize.y * .5f) - position.y;
		if (d <= 0.f)
		{
			velocity.y *= -1.f;
			position.y += d;
		}
	}
	// players
	for (int i = 0; i < 2; ++i)
	{
		if (_checkPlayerCollision(_gs.players[i]))
		{
			float d = ((playerSize + ballSize) * .5f).y - std::fabs(position.y - _gs.players[i].position.y);
			if (velocity.y > 0.f) { position.y -= d; }
			else { position.y += d; }
			velocity.y *= -1.f;
		}
	}

	// then horizontal
	position.x += velocity.x;
	// left border
	{
		float d = ballSize.x * .5f - position.x;
		if (d >= 0.f)
		{
			velocity.x *= -1.f;
			position.x += d;
		}
	}
	// right border
	{
		float d = (gameSize.x - ballSize.x * .5f) - position.x;
		if (d <= 0.f)
		{
			velocity.x *= -1.f;
			position.x += d;
		}
	}
	// players
	for (int i = 0; i < 2; ++i)
	{
		if (_checkPlayerCollision(_gs.players[i]))
		{
			float d = ((playerSize + ballSize) * .5f).x - std::fabs(position.x - _gs.players[i].position.x);
			if (velocity.x > 0.f) { position.x -= d; }
			else { position.x += d; }
			velocity.x *= -1.f;
		}
	}
}

void Ball::draw(sf::RenderWindow& _window)
{
	sf::RectangleShape shape(ballSize);
	shape.setFillColor(sf::Color::White);
	shape.setPosition(position - ballSize * .5f);
	_window.draw(shape);
}

bool Ball::_checkPlayerCollision(const Player& _player)
{
	float l = position.x - ballSize.x * .5f;
	float r = position.x + ballSize.x * .5f;
	float t = position.y - ballSize.y * .5f;
	float b = position.y + ballSize.y * .5f;

	float pl = _player.position.x - playerSize.x * .5f;
	float pr = _player.position.x + playerSize.x * .5f;
	float pt = _player.position.y - playerSize.y * .5f;
	float pb = _player.position.y + playerSize.y * .5f;

	return l < pr && r > pl && t < pb && b > pt;
}
