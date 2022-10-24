#include "crate.h"
#include <engine/sprite.h>
#include <engine/renderer.h>
#include <engine/scene.h>

Crate::Crate()
	: m_bRunPhysics(true)
{

}

Crate::~Crate()
{

}

bool
Crate::Initialise(Renderer& renderer, b2World& physWorld, FMOD::System& fmodSystem)
{
	if (!Entity::Initialise(renderer, physWorld, fmodSystem))
	{
		return false;
	}

	m_pCrateSprite = renderer.CreateSprite("assets\\sprites\\crate.png", SpriteOrigin::CENTER);
	m_pb2World = &physWorld;

	return true;
}

void 
Crate::Reset(Vector2 position)
{
	b2BodyDef bodyDefCrate;
	bodyDefCrate.type = b2_dynamicBody;
	bodyDefCrate.position.Set(position.x, position.y);
	m_pPhysBody = m_pb2World->CreateBody(&bodyDefCrate);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(Renderer::ScreenSpaceToWorldSpace(m_pCrateSprite->GetWidth()) / 2.f, Renderer::ScreenSpaceToWorldSpace(m_pCrateSprite->GetHeight()) / 2.f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	m_pPhysBody->CreateFixture(&fixtureDef);
}

Vector2
Crate::GetPosition()
{
	return Vector2::FromB2Vec(m_pPhysBody->GetPosition());
}

void 
Crate::Process(float deltaTime, Input& input)
{
	if (!m_bRunPhysics)
	{
		m_pPhysBody->SetLinearVelocity({ 0.f, 0.f });
		m_pPhysBody->SetAngularVelocity(0.f);
	}
}

void 
Crate::Draw(Renderer& renderer)
{
	m_pCrateSprite->Draw(renderer, GetPosition(), m_pPhysBody->GetAngle());
}

#ifdef _DEBUG
void 
Crate::DebugDraw()
{

}
#endif // _DEBUG
