#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <algorithm>

#include <imgui-SFML.h>
#include <imgui.h>


#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

sf::Vector2f gameSize(640.f, 480.f);
sf::Vector2f playerSize(15.f, 100.f);
sf::Vector2f ballSize(10.f, 10.f);
float playerBorderOffset = 50.f;
float playerSpeed = 6.f;
sf::Vector2f ballInitialVelocity = sf::Vector2f(6.f, 6.f);

struct Player
{
	sf::Vector2f position;
	bool upPressed = false;
	bool downPressed = false;

	void update()
	{
		float direction = 0.f;
		if (upPressed) direction -= 1.f;
		if (downPressed) direction += 1.f;
		position.y += direction * playerSpeed;
		position.y = std::max(std::min(position.y, gameSize.y - playerSize.y * .5f), playerSize.y * .5f);
	}

	void draw(sf::RenderWindow& _window)
	{
		sf::RectangleShape shape(playerSize);
		shape.setFillColor(sf::Color::White);
		shape.setPosition(position - playerSize * .5f);
		_window.draw(shape);
	}
};
Player p1;
Player p2;
Player* players[2] = { &p1, &p2 };

struct Ball
{
	sf::Vector2f position;
	sf::Vector2f velocity;

	void update()
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
			if (checkPlayerCollision(*players[i]))
			{
				float d = ((playerSize + ballSize) * .5f).y - std::fabs(position.y - players[i]->position.y);
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
			if (checkPlayerCollision(*players[i]))
			{
				float d = ((playerSize + ballSize) * .5f).x - std::fabs(position.x - players[i]->position.x);
				if (velocity.x > 0.f) { position.x -= d; }
				else { position.x += d; }
				velocity.x *= -1.f;
			}
		}
	}

	bool checkPlayerCollision(const Player& _player)
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

	void draw(sf::RenderWindow& _window)
	{
		sf::RectangleShape shape(ballSize);
		shape.setFillColor(sf::Color::White);
		shape.setPosition(position - ballSize * .5f);
		_window.draw(shape);
	}
};

Ball ball;

int main( int _argc, const char* _argv[] )
{
	sf::RenderWindow window(sf::VideoMode(int(gameSize.x), int(gameSize.y)), "GGPOng");

	//window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	ImGui::SFML::Init(window);


	p1.position = sf::Vector2f(playerBorderOffset, gameSize.y * .5f);
	p2.position = sf::Vector2f(gameSize.x - playerBorderOffset, gameSize.y * .5f);

	ball.position = gameSize * .5f;
	ball.velocity = ballInitialVelocity;

	sf::Clock deltaClock;
	while (window.isOpen()) {
		// INPUT
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { p1.upPressed = true; }
				if (event.key.code == sf::Keyboard::Key::Down) { p1.downPressed = true;	}

				if (event.key.code == sf::Keyboard::Key::E) { p2.upPressed = true; }
				if (event.key.code == sf::Keyboard::Key::D) { p2.downPressed = true; }
			}
			
			if (event.type == sf::Event::KeyReleased)
			{
				if (event.key.code == sf::Keyboard::Key::Up) { p1.upPressed = false; }
				if (event.key.code == sf::Keyboard::Key::Down) { p1.downPressed = false; }

				if (event.key.code == sf::Keyboard::Key::E) { p2.upPressed = false; }
				if (event.key.code == sf::Keyboard::Key::D) { p2.downPressed = false; }
			}

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// UPDATE
		p1.update();
		p2.update();
		ball.update();

		ImGui::SFML::Update(window, deltaClock.restart());

		// DRAW
		window.clear();

		p1.draw(window);
		p2.draw(window);
		ball.draw(window);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();

	return EXIT_SUCCESS;
}
