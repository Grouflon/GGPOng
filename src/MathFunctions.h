#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

#define PI (3.141592653589793f)

#define R2D (180.f / PI)
#define D2R (PI / 180.f)

float Sign(float _v);

float Vector2_Dot(const sf::Vector2f& _a, const sf::Vector2f& _b);
float Vector2_Len(const sf::Vector2f& _v);
sf::Vector2f Vector2_Normalize(const sf::Vector2f& _v);
sf::Vector2f Vector2_Rotate(const sf::Vector2f& _v, float _angleRad);
float Vector2_SignedAngleBetween(const sf::Vector2f& _a, const sf::Vector2f& _b);
