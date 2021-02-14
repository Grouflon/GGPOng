#include "GameplayState.h"	

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <Globals.h>
#include <MathFunctions.h>

std::uniform_real_distribution<float> dis(0.0f, 1.0f);

void GameplayState::init()
{
	frameNumber = 0;
	players[0].position = sf::Vector2f(playerBorderOffset, gameSize.y * .5f);
	players[1].position = sf::Vector2f(gameSize.x - playerBorderOffset, gameSize.y * .5f);

	reset();
}

void GameplayState::reset()
{
	rng = std::mt19937(frameNumber);

	justScoredPlayer = -1;
	frameNumber = 0;
	score[0] = 0;
	score[1] = 0;
	stateTimer = 0;

	setState(GameState::GS_Engage);
}

void GameplayState::update(int _input[2])
{
	++frameNumber;

	if (state == GameState::GS_Engage && (_input[0] != 0 || _input[1] != 0))
	{
		setState(GameState::GS_Playing);
	}

	for (int i = 0; i < 2; ++i)
	{
		players[i].update(_input[i]);
	}

	if (state == GS_Playing)
	{
		ball.update(*this);

		if (ball.position.x < -ballSize.x)
		{
			++score[1];
			setState(GS_Scored);
			justScoredPlayer = 1;
		}
		else if (ball.position.x > gameSize.x + ballSize.x)
		{
			justScoredPlayer = 0;
			++score[0];
			setState(GS_Scored);
		}
	}

	if (state != GameState::GS_GameOver && (score[0] >= scoreLimit || score[1] >= scoreLimit))
	{
		setState(GS_GameOver);
	}

	if (stateTimer > 0)
	{
		--stateTimer;
	}

	if (state == GameState::GS_Scored && stateTimer == 0)
	{
		setState(GS_Engage);
	}

	if (state == GameState::GS_GameOver && stateTimer == 0)
	{
		reset();
	}
}

void GameplayState::draw(sf::RenderWindow& _window)
{
	players[0].draw(_window);
	players[1].draw(_window);
	ball.draw(_window);

	if (state == GameState::GS_Scored || state == GameState::GS_GameOver)
	{
		char buf1[16];
		const char * buf2 = ":";
		char buf3[16];
		sprintf(buf1, "%d", score[0]);
		sprintf(buf3, "%d", score[1]);
		int characterSize = 250;
		sf::Text text1(buf1, titleFont, characterSize);
		sf::Text text2(buf2, titleFont, characterSize);
		sf::Text text3(buf3, titleFont, characterSize);
		text1.setFillColor(sf::Color(255, 255, 255, justScoredPlayer == 0 ? 255 : 127));
		text2.setFillColor(sf::Color(255, 255, 255, 255));
		text3.setFillColor(sf::Color(255, 255, 255, justScoredPlayer == 1 ? 255 : 127));

		sf::Vector2f textCenter = gameSize * .5f;
		textCenter.y -= 175.f;
		textCenter.x -= 15.f;
		text2.setPosition(textCenter);

		float textOffset = 120.f;
		float charOffset = 74.f;
		text1.setPosition(textCenter + sf::Vector2f(-textOffset - charOffset, 0.f));
		text3.setPosition(textCenter + sf::Vector2f(textOffset - charOffset, 0.f));

		_window.draw(text1);
		_window.draw(text2);
		_window.draw(text3);
	}

	if (state == GameState::GS_GameOver)
	{
		sf::Text text("GAME OVER", titleFont, 20);
		sf::Vector2f textCenter = gameSize * .5f - text.getLocalBounds().getSize() * .5f;
		textCenter.y += 100.f;
		text.setFillColor(sf::Color(255, 255, 255, 255));
		text.setPosition(textCenter);

		_window.draw(text);
	}
}

void GameplayState::setState(GameState _state)
{
	if (state == _state)
		return;

	// Exit state
	switch (state)
	{
	case GS_None:
		break;
	case GS_Engage:
		break;
	case GS_Playing:
		break;
	case GS_Scored:
		break;
	case GS_GameOver:
		break;
	default:
		break;
	}

	auto gsToString = [](GameState _state)-> const char*
	{
		switch (_state)
		{
		case GS_None:
			return "None";
			break;
		case GS_Engage:
			return "Engage";
			break;
		case GS_Playing:
			return "Playing";
			break;
		case GS_Scored:
			return "Scored";
			break;
		case GS_GameOver:
			return "GameOver";
			break;
		default:
			break;
		}
		return "";
	};

	//printf("%s -> %s\n", gsToString(state), gsToString(_state));
	state = _state;

	// Enter state
	switch (state)
	{
	case GS_None:
	break;
	case GS_Engage:
	{
		ball.position = gameSize * .5f;

		float rng1 = dis(rng);
		float rng2 = dis(rng);
		float sign = rng1 > 0.5f ? 1.f : -1.f;
		float angle = rng2 * ballMaxAngle * 2.f - ballMaxAngle;
		sf::Vector2f initialVelocity = Vector2_Rotate(sf::Vector2f(1.f, 0.f), angle * D2R);
		initialVelocity.x *= sign;
		initialVelocity *= ballSpeed;

		ball.velocity = initialVelocity;
	}
	break;
	case GS_Playing:
	{

	}
	break;
	case GS_Scored:
	{
		stateTimer = 150;
	}
	break;
	case GS_GameOver:
	{
		stateTimer = 250;
	}
	break;
	default:
		break;
	}
}

