#pragma once

#include "Core/Containers/FixedSizePool.h"

template<class BaseType> 
class SyncFixedSizePool final : public FixedSizePool<BaseType> 
{
public:
	SyncFixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements);
	~SyncFixedSizePool() = default;

	void* Allocate(unsigned int size) final;
	void Deallocate(BaseType* ptr) final;

private:
	std::mutex m_mutex;
};

template<class BaseType> 
SyncFixedSizePool<BaseType>::SyncFixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements) 
	: FixedSizePool<BaseType>(poolName, maxElementSize, maxElements)
{
}

template<class BaseType> void* SyncFixedSizePool<BaseType>::Allocate(unsigned int size) 
{
	std::scoped_lock<std::mutex> lock(m_mutex);
	return FixedSizePool<BaseType>::Allocate(size);
}

template<class BaseType> void SyncFixedSizePool<BaseType>::Deallocate(BaseType* ptr) 
{
	std::scoped_lock<std::mutex> lock(m_mutex);
	FixedSizePool<BaseType>::Deallocate(ptr);
}