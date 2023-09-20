#pragma once

#include "Core/Logging/Log.h"

/**
* Pool which allocate a fixed size of memory defined in constructor argument. This pool offers high performance until the maximum
* elements defined in constructor is not reached. Once maximum number of element reached, the performance strongly decrease.
*/
template<class BaseType>
class FixedSizePool
{
public:
	FixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements);
	FixedSizePool(const FixedSizePool&) = delete;
	FixedSizePool& operator=(const FixedSizePool&) = delete;
	virtual ~FixedSizePool();

	virtual void* Allocate(unsigned int);
	virtual void Deallocate(BaseType*);

private:
	void logPoolIsFull();

	std::string m_poolName;
	unsigned int m_maxElementSize;
	unsigned int m_maxElements;
	unsigned int m_freeCount; //number of free locations

	std::byte* m_pool;
	void* m_firstFree;

	bool m_fullPoolLogged;
};

/**
* @param maxElementSize Size of element to store in pool. If there are several classes which extends 'BaseType', the biggest one should be provided
* @param maxElement Maximum of elements which can be stored in the pool. If the maximum is exceed, a classical allocation (new/delete) will be performed.
*/
template<class BaseType> 
FixedSizePool<BaseType>::FixedSizePool(const std::string& poolName, unsigned int maxElementSize, unsigned int maxElements) 
	: m_poolName(poolName)
	, m_maxElementSize(maxElementSize)
	, m_maxElements(maxElements)
	, m_freeCount(maxElements)
	, m_fullPoolLogged(false) 
{
	//create pool
	unsigned int poolSize = m_maxElementSize * m_maxElements;
	assert(poolSize != 0 && "Impossible to create a pool size of 0");
	m_pool = static_cast<std::byte*>(malloc(poolSize));
	m_firstFree = m_pool;

	//initialize pool: each element contains address of next element and last one contains 0
	std::byte* p = m_pool;
	auto count = (int)m_maxElements;
	while (--count) 
	{
		*(void**)p = (p + m_maxElementSize);
		p += m_maxElementSize;
	}
	*(void**)p = nullptr;
}

template<class BaseType> 
FixedSizePool<BaseType>::~FixedSizePool() 
{
	if (m_freeCount != m_maxElements)
	{ //ensure that 'free' method has been called
		LogError("Fixed size pool '" + m_poolName + "' not correctly cleared. Free count: " + std::to_string(m_freeCount) + ", max elements: " + std::to_string(m_maxElements) + ".");
	}

	free(m_pool);
}

// return Memory pointer which can be used to instantiate an element.
template<class BaseType> 
void* FixedSizePool<BaseType>::Allocate(unsigned int size) 
{
	if (size > m_maxElementSize)
	{
		LogError("Fixed size pool '" + m_poolName + "' cannot allocate " + std::to_string(size) + " bytes because max allowed allocation is " + std::to_string(m_maxElementSize) + " bytes");
		return nullptr;
	}

	if (m_freeCount != 0)
	{ //pool is not full
		void* result = m_firstFree;
		m_firstFree = *(void**)m_firstFree;
		--m_freeCount;

		return result;
	}

	// pool is full: allocate new memory location
	logPoolIsFull();
	return operator new(m_maxElementSize);
}

// Call destructor of pointer and free location in the pool.
template<class BaseType>
void FixedSizePool<BaseType>::Deallocate(BaseType* ptr) 
{
	if (reinterpret_cast<std::byte*>(ptr) >= m_pool && reinterpret_cast<std::byte*>(ptr) < m_pool + m_maxElementSize * m_maxElements) 
	{ //ptr is in the pool
		ptr->~BaseType();

		*(void**)ptr = m_firstFree;
		m_firstFree = ptr;
		++m_freeCount;
	}
	else
	{
		delete ptr;
	}
}

template<class BaseType> 
void FixedSizePool<BaseType>::logPoolIsFull()
{
	if (!m_fullPoolLogged)
	{
		std::stringstream logStream;
		logStream << "Pool is full of elements." << std::endl;
		logStream << " - Pool name: " << m_poolName << std::endl;
		logStream << " - Element size: " << m_maxElementSize << std::endl;
		logStream << " - Maximum elements: " << m_maxElements;
		LogWarning(logStream.str());

		m_fullPoolLogged = true;
	}
}