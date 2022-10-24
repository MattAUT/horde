#ifndef __NODE_H__
#define __NODE_H__

class Sprite;
class Renderer;
class Player;

#include <engine/id.h>
#include <vector2.h>

namespace INI 
{
	class Settings;
}

class Node
{
public:
	Node();
	~Node();
	bool Initialise(Renderer& renderer, INI::Settings levelSettings, unsigned int index);
	void Setup(Node*& nodes);
	ID GetId() const;
	Vector2 GetPosition() const;
	unsigned int GetConnectionCount() const;
	Node** GetConnections();

protected:
private:
	Node(const Node& n);
	Node& operator=(const Node& n) {}

public:
protected:
	ID m_id;
	Vector2 m_position;

	unsigned int m_uiConnCount;
	unsigned int* m_connectionIndicies;
	Node** m_pConnections;
private:

};

#endif // __NODE_H__