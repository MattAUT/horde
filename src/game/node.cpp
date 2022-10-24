#include "node.h"

#include <engine/renderer.h>
#include <engine/sprite.h>
#include <engine/scene.h>
#include <logmanager.h>
#include <ini.h>

#include "player.h"

Node::Node()
{
	m_id = ID();
	LogManager::GetInstance().Log("Node Generated with ID: " + m_id.AsString());
}

Node::~Node()
{
	delete[] m_pConnections;
	delete[] m_connectionIndicies;

	m_pConnections = nullptr;
	m_connectionIndicies = nullptr;
}

bool 
Node::Initialise(Renderer& renderer, INI::Settings levelSettings, unsigned int index)
{
	char xPos[16];
	char yPos[16]; 
	char connCountPath[16];

	sprintf_s(xPos, "%dx", index);
	sprintf_s(yPos, "%dy", index);
	sprintf_s(connCountPath, "%dconn_count", index);

	m_position =
	{
		levelSettings.GetFloat("path_nodes", xPos),
		levelSettings.GetFloat("path_nodes", yPos)
	};

	m_uiConnCount = levelSettings.GetInt("path_nodes", connCountPath);
	m_connectionIndicies = new unsigned int[m_uiConnCount];
	m_pConnections = new Node*[m_uiConnCount];

	for (unsigned int k = 0; k < m_uiConnCount; ++k)
	{
		char connIndexPath[16];
		sprintf_s(connIndexPath, "%dconn%di", index, k);
		m_connectionIndicies[k] = levelSettings.GetInt("path_nodes", connIndexPath);
	}

	return true;
}

void
Node::Setup(Node*& nodes)
{
	for (unsigned int i = 0; i < m_uiConnCount; ++i)
	{
 		m_pConnections[i] = &nodes[m_connectionIndicies[i]];
	}
}

ID
Node::GetId() const
{
	return m_id;
}

Vector2
Node::GetPosition() const
{
	return m_position;
}

Node**
Node::GetConnections()
{
	return m_pConnections;
}

unsigned int
Node::GetConnectionCount() const
{
	return m_uiConnCount;
}
