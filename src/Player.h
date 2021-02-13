#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf { class RenderWindow; }

struct Player
{
	sf::Vector2f position;

	void update(int _input);
	void draw(sf::RenderWindow& _window);
};