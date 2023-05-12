#pragma once

#include "ISubsystem.h"

class SubsystemManager
{
public:
	template<typename T>
	void RegisterSubsystem()
	{
		auto subsystem = std::make_shared<T>();
		const size_t freeid = m_subsystems.size();
		subsystem->registerSubsystem(static_cast<uint16_t>(freeid), std::shared_ptr<SubsystemManager>(this));
		m_subsystems.emplace_back(subsystem);
	}

	template<typename T>
	std::shared_ptr<T> GetSubsystem()
	{
		assert(T::getId() < m_subsystems.size());
		return std::dynamic_pointer_cast<T>(m_subsystems[T::getId()]);
	}

private:
	std::vector<std::shared_ptr<ISubsystem>> m_subsystems;
};

template<typename T>
inline std::shared_ptr<T> ISubsystem::GetSubsystem()
{
	assert(m_managerRef);
	return m_managerRef->GetSubsystem<T>();
}