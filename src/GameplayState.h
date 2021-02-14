#pragma once

#include <random>

#include <Player.h>
#include <Ball.h>

enum GameState
{
	GS_None,
	GS_Engage,
	GS_Playing,
	GS_Scored,
	GS_GameOver
};

struct GameplayState
{
	Player players[2];
	Ball ball;

	int frameNumber;
	GameState state = GS_None;
	int score[2];
	int justScoredPlayer;
	int stateTimer;

	std::mt19937 rng;

	void init();
	void reset();
	void update(int _input[2]);
	void draw(sf::RenderWindow& _window);

	void setState(GameState _state);
};
