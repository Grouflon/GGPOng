#include "Player.h"

#include <algorithm>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <Globals.h>

void Player::update(int _input)
{
	float direction = 0.f;
	if (_input & 1) direction -= 1.f;
	if (_input & 2) direction += 1.f;
	position.y += direction * playerSpeed;
	position.y = std::max(std::min(position.y, gameSize.y - playerSize.y * .5f), playerSize.y * .5f);
}

void Player::draw(sf::RenderWindow& _window)
{
	sf::RectangleShape shape(playerSize);
	shape.setFillColor(sf::Color::White);
	shape.setPosition(position - playerSize * .5f);
	_window.draw(shape);
}
