#pragma once

#if USE_PHYSICS

#include "Core/Containers/SyncFixedSizePool.h"
#include "Core/Pattern/Singleton.h"

class CollisionConvexObject3D;

class CollisionConvexObjectPool : public ThreadSafeSingleton<CollisionConvexObjectPool>
{
public:
	friend class ThreadSafeSingleton<CollisionConvexObjectPool>;

	CollisionConvexObjectPool();
	~CollisionConvexObjectPool() override = default;

	SyncFixedSizePool<CollisionConvexObject3D>& getObjectsPool();

private:
	unsigned int maxObjectSize(const std::vector<unsigned int>&) const;

	std::unique_ptr<SyncFixedSizePool<CollisionConvexObject3D>> m_objectsPool;
};

#endif // USE_PHYSICS