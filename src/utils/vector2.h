// COMP710 GP Framework 2022
#ifndef __VECTOR2_H__
#define __VECTOR2_H__

#include <box2d/box2d.h>

enum COORD_SYSTEM
{
	CARTETIAN,
	POLAR
};

// Class declaration: 
class Vector2
{
	// Member methods: 
public:
	Vector2();
	Vector2(float inx, float iny);
	Vector2(float angle, float distance, COORD_SYSTEM system);
	~Vector2();
	void Set(float inx, float iny);
	float LengthSquared() const;
	float Length() const;
	Vector2 Normalise();

	friend Vector2 operator+(const Vector2& veca, const Vector2& vecb)
	{
		return (Vector2(veca.x + vecb.x, veca.y + vecb.y));
	}

	friend Vector2 operator-(const Vector2& veca, const Vector2& vecb)
	{
		return (Vector2(veca.x - vecb.x, veca.y - vecb.y));
	}

	friend Vector2 operator*(float scalar, const Vector2& vec)
	{
		return (Vector2(vec.x * scalar, vec.y * scalar));
	}

	friend Vector2 operator*(const Vector2& vec, float scalar)
	{
		return (Vector2(vec.x * scalar, vec.y * scalar));
	}

	friend Vector2 operator/(const Vector2& vec, float scalar)
	{
		return (Vector2(vec.x / scalar, vec.y / scalar));
	}

	friend Vector2 operator/(float scalar, const Vector2& vec)
	{
		return (Vector2(vec.x / scalar, vec.y / scalar));
	}

	Vector2& operator*=(float scalar)
	{
		x = x * scalar;
		y = y * scalar;
		return (*this);
	}

	Vector2& operator+=(const Vector2& vecRight)
	{
		x = x + vecRight.x;
		y = y + vecRight.y;
		return (*this);
	}
	Vector2& operator-=(const Vector2& vecRight)
	{
		x = x - vecRight.x;
		y = y - vecRight.y;
		return (*this);
	}

	bool operator==(const Vector2& other)
	{
		return x == other.x && y == other.y;
	}

	static float DotProduct(const Vector2& veca, const Vector2& vecb);
	static Vector2 Lerp(const Vector2& veca, const Vector2& vecb, float time);
	static Vector2 Reflect(const Vector2& veca, const Vector2& vecb);
	static Vector2 Clamp(float minLength, const Vector2& vec, float maxLength);
	static Vector2 FromB2Vec(const b2Vec2 vec);

protected:
private:
	//    Vector2(const Vector2& vector2);
	//    Vector2& operator=(const Vector2& vector2);

	// Member data: 
public:
	float x;
	float y;

protected:
private:
};

#endif // __VECTOR2_H__