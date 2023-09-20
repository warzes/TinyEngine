#include "stdafx.h"
#if USE_PHYSICS
#include "CollisionConvexObject.h"
#include "CollisionConvexObjectPool.h"
//-----------------------------------------------------------------------------
void ObjectDeleter::operator()(CollisionConvexObject3D* const collisionAlgorithm) const 
{
	CollisionConvexObjectPool::Instance().getObjectsPool().Deallocate(collisionAlgorithm);
}
//-----------------------------------------------------------------------------
#endif // USE_PHYSICS