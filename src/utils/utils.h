#ifndef __UTILS_H__
#define __UTILS_H__

#ifndef M_PI
#define M_PI 3.1415926535897932384
#endif // M_PI

#include <cmath>
#include <cstdlib>
#include <vector2.h>

inline int GetRandom(int lowerBoundary, int upperBoundary)
{
	int range = (upperBoundary - lowerBoundary + 1);
	return (rand() % range) + lowerBoundary;
}

inline float GetRandom_f(float lowerBoundary, float upperBoundary)
{
	float range = 1000;
	int rand_i = GetRandom(0, static_cast<int>(range));
	float newRange = upperBoundary - lowerBoundary;
	return (static_cast<float>(rand_i) * newRange) / range + lowerBoundary;
}

inline float GetPositiveOrNegative()
{
	int coinToss = rand() % 2;
	float result = (coinToss == 0) ? -1.0f : 1.0f;
	return result;
}

inline float DegToRad(float degrees)
{
	return degrees * static_cast<float>(M_PI) / 180.f;
}

inline float RadToDeg(float degrees)
{
	return degrees * 180.f / static_cast<float>(M_PI);
}

inline float Clamp(float minimum, float value, float maximum)
{
	if (value > maximum)
	{
		value = maximum;
	}
	else if (value < minimum)
	{
		value = minimum;
	}

	return value;
}

inline float AngleClamp(float angle)
{
	while (angle > 360)
	{
		angle -= 360;
	}

	while (angle < 0)
	{
		angle += 360;
	}

	return angle;
}

inline float EaseInOutQuad(float time, float start, float change, float duration)
{
	if (time < 0)
	{
		return start;
	}

	time /= (duration / 2);
	if (time < 1.0f)
	{
		return ((change / 2.0f) * (time * time)) + start;
	}
	return -change / 2.0f * (((time - 2.0f) * (--time)) - 1.0f) + start;
}

inline float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

#endif // __UTILS_H__