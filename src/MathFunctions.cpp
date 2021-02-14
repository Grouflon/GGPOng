#include "MathFunctions.h"

float Sign(float _v)
{
	if (_v > 0.f)
		return 1.f;
	else if (_v < 0.f)
		return -1.f;
	return 0.f;
}

float Vector2_Dot(const sf::Vector2f& _a, const sf::Vector2f& _b)
{
	return _a.x * _b.x + _a.y * _b.y;
}

float Vector2_Len(const sf::Vector2f& _v)
{
	return sqrt(_v.x * _v.x + _v.y * _v.y);
}

sf::Vector2f Vector2_Normalize(const sf::Vector2f& _v)
{
	float len = Vector2_Len(_v);
	if (len > 0.00001f)
	{
		return _v / len;
	}
	return sf::Vector2f(0.f, 0.f);
}

sf::Vector2f Vector2_Rotate(const sf::Vector2f& _v, float _angleRad)
{
	float sine = std::sin(_angleRad);
	float cosine = std::cos(_angleRad);
	return sf::Vector2f(_v.x * cosine - _v.y * sine, _v.x * sine + _v.y * cosine);
}

float Vector2_SignedAngleBetween(const sf::Vector2f& _a, const sf::Vector2f& _b)
{
	float sign = -Sign(_a.x * _a.y - _a.y * _b.x);
	return sign * std::acos(Vector2_Dot(Vector2_Normalize(_a), Vector2_Normalize(_b)));
}
