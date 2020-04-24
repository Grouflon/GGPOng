#pragma once

#include <SFML/System/Vector2.hpp>

namespace sf { class RenderWindow; }

struct Player
{
	sf::Vector2f position;
	bool upPressed = false;
	bool downPressed = false;

	void update();
	void draw(sf::RenderWindow& _window);
};