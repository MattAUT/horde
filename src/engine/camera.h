#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <vector2.h>

class Camera
{
public:
	Camera();
	~Camera();

	Vector2 GetPosition() const;
	void SetPosition(Vector2 position);
protected:
private:

public:
protected:
	Vector2 m_position;
private:
};

#endif // __CAMERA_H__