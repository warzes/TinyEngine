#include "stdafx.h"
#if USE_PHYSICS
#include "PhysicsSystem.h"
#include "Core/Logging/Log.h"
//-----------------------------------------------------------------------------
PhysicsSystem gPhysicsSystem;
//-----------------------------------------------------------------------------
bool PhysicsSystem::Create(const PhysicsCreateInfo& createInfo)
{
	LogPrint("PhysicsSystem Create");

	m_gravity = createInfo.gravity;

	return true;
}
//-----------------------------------------------------------------------------
void PhysicsSystem::Destroy()
{
}
//-----------------------------------------------------------------------------
void PhysicsSystem::Update()
{
}
//-----------------------------------------------------------------------------
void PhysicsSystem::SetGravity(const glm::vec3& gravity)
{
	m_gravity = gravity;
}
//-----------------------------------------------------------------------------
glm::vec3 PhysicsSystem::GetGravity() const
{
	return m_gravity;
}
//-----------------------------------------------------------------------------
PhysicsSystem& GetPhysicsSystem()
{
	return gPhysicsSystem;
}
//-----------------------------------------------------------------------------
#endif // USE_PHYSICS