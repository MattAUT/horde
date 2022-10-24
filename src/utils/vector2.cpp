// COMP710 GP Framework 2022
// This include: 
#include "vector2.h"

// Library includes: 
#include <cassert>
#include <cmath>

Vector2::Vector2()
	: x(0.0f)
	, y(0.0f)
{

}

Vector2::~Vector2()
{

}

Vector2::Vector2(float inx, float iny)
	: x(inx)
	, y(iny)
{

}

Vector2::Vector2(float angle, float distance, COORD_SYSTEM system)
	: x{ system == POLAR ? distance * cosf(angle) : angle }
	, y{ system == POLAR ? distance * sinf(angle) : distance }
{

}

void
Vector2::Set(float inx, float iny)
{
	x = inx;
	y = iny;
}

float
Vector2::LengthSquared() const
{
	return ((x * x) + (y * y));
}

float
Vector2::Length() const
{
	return (sqrtf(LengthSquared()));
}

Vector2
Vector2::Normalise()
{
	float length = Length();
	return { x / length, y / length };
}

float
Vector2::DotProduct(const Vector2& veca, const Vector2& vecb)
{
	return (veca.x * vecb.x) + (veca.y * vecb.y);
}

Vector2
Vector2::Lerp(const Vector2& veca, const Vector2& vecb, float time)
{
	return (Vector2(veca + time * (vecb - veca)));
}

Vector2
Vector2::Reflect(const Vector2& vec, const Vector2& normal)
{
	return (vec - 2.0f * Vector2::DotProduct(vec, normal) * normal);
}

Vector2 
Vector2::Clamp(float minLength, const Vector2& vec, float maxLength)
{
	if (vec.LengthSquared() > powf(maxLength, 2))
	{
		return { atan2f(vec.y, vec.x), maxLength, COORD_SYSTEM::POLAR };
	}

	if (vec.LengthSquared() < powf(minLength, 2))
	{
		return { atan2f(vec.y, vec.x), minLength, COORD_SYSTEM::POLAR };
	}

	return vec;
}

Vector2 
Vector2::FromB2Vec(const b2Vec2 vec)
{
	return { vec.x, vec.y };
}