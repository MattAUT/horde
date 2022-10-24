#include "camera.h"

Camera::Camera()
	: m_position{ 0, 0 }
{

}

Camera::~Camera()
{

}

Vector2
Camera::GetPosition() const
{
	return m_position;
}

void 
Camera::SetPosition(Vector2 position)
{
	m_position = { -position.x, position.y };
}