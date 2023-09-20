#include "stdafx.h"
#include "Singleton.h"
//-----------------------------------------------------------------------------
std::vector<SingletonData> SingletonContainer::m_singletonsVector;
//-----------------------------------------------------------------------------
void SingletonContainer::RegisterSingleton(const std::string& name, std::unique_ptr<SingletonInterface> ptr) 
{
#if defined(_DEBUG)
	assert(std::ranges::count_if(m_singletonsVector, [&name](const SingletonData& sa) { return sa.name == name; }) == 0);
#endif
	m_singletonsVector.emplace_back(SingletonData{ .name = name, .ptr = std::move(ptr) });
}
//-----------------------------------------------------------------------------
void SingletonContainer::DestroyAllSingletons()
{
	for (int i = (int)m_singletonsVector.size() - 1; i >= 0; --i)
		m_singletonsVector.resize((std::size_t)i); //destroy singletons in FILO
}
//-----------------------------------------------------------------------------