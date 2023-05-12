#pragma once

class SubsystemManager;

#define SUBSYSTEMIMPL \
	private: \
		friend class SubsystemManager; \
		static inline uint16_t m_id = std::numeric_limits<uint16_t>::max(); \
		static uint16_t getId() { return m_id; } \
		void registerSubsystem(uint16_t nid, std::shared_ptr<SubsystemManager> manager) { m_id = nid; m_managerRef = manager; }

class ISubsystem
{
	friend class SubsystemManager;
public:
	virtual ~ISubsystem() = default;

	template<typename T>
	std::shared_ptr<T> GetSubsystem();

	bool IsRegister() const { return m_managerRef != nullptr; }

protected:
	ISubsystem() = default;
	ISubsystem(ISubsystem&&) = delete;
	ISubsystem(const ISubsystem&) = delete;
	ISubsystem& operator=(ISubsystem&&) = delete;
	ISubsystem& operator=(const ISubsystem&) = delete;

	std::shared_ptr<SubsystemManager> m_managerRef = nullptr;
};