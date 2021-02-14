#pragma once

#include <SFML/System/Vector2.hpp>

struct Player;
struct GameplayState;
namespace sf { class RenderWindow; }


struct Ball
{
	sf::Vector2f position;
	sf::Vector2f velocity;

	void update(GameplayState& _gs);
	void draw(sf::RenderWindow& _window);

private:
	bool _checkPlayerCollision(const Player& _player);
};