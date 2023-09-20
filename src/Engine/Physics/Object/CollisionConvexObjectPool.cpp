#include "stdafx.h"
#if USE_PHYSICS
#include "CollisionConvexObjectPool.h"
//-----------------------------------------------------------------------------
CollisionConvexObjectPool::CollisionConvexObjectPool() 
{
	unsigned int maxElementSize = maxObjectSize({
	sizeof(CollisionBoxObject),
	sizeof(CollisionCapsuleObject),
	sizeof(CollisionConeObject),
	sizeof(CollisionConvexHullObject),
	sizeof(CollisionCylinderObject),
	sizeof(CollisionSphereObject),
	sizeof(CollisionTriangleObject)
		});
	unsigned int objectsPoolSize = ConfigService::instance().getUnsignedIntValue("collisionObject.poolSize");

	//pool is synchronized because elements are created in narrow phase (= synchronized phase called by different threads) and deleted by different threads outside the narrow phase
	m_objectsPool = std::make_unique<SyncFixedSizePool<CollisionConvexObject3D>>("collisionConvexObjectsPool", maxElementSize, objectsPoolSize);
}
//-----------------------------------------------------------------------------
SyncFixedSizePool<CollisionConvexObject3D>& CollisionConvexObjectPool::getObjectsPool() 
{
	return *m_objectsPool;
}
//-----------------------------------------------------------------------------
unsigned int CollisionConvexObjectPool::maxObjectSize(const std::vector<unsigned int>& objectsSize) const 
{
	unsigned int result = 0;
	for (unsigned int objectSize : objectsSize)
		result = std::max(result, objectSize);

	return result;
}
//-----------------------------------------------------------------------------
#endif // USE_PHYSICS