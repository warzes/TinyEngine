#pragma once

struct PhysicsCreateInfo final
{
	glm::vec3 gravity = { 0.0f, -9.8f, 0.0f };
	bool enable = false;
};

#if USE_PHYSICS

class PhysicsSystem final
{
	friend class EngineDevice;
public:
	PhysicsSystem() = default;

	bool Create(const PhysicsCreateInfo& createInfo);
	void Destroy();

	void Update();

	void SetGravity(const glm::vec3& gravity);
	glm::vec3 GetGravity() const;

private:
	PhysicsSystem(PhysicsSystem&&) = delete;
	PhysicsSystem(const PhysicsSystem&) = delete;
	PhysicsSystem& operator=(PhysicsSystem&&) = delete;
	PhysicsSystem& operator=(const PhysicsSystem&) = delete;

	glm::vec3 m_gravity;
};

PhysicsSystem& GetPhysicsSystem();

#endif // USE_PHYSICS