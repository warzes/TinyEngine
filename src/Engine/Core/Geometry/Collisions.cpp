#include "stdafx.h"
#include "Collisions.h"
#include <ccd/src/ccd/ccd.h>

//-----------------------------------------------------------------------------
// CCD
//-----------------------------------------------------------------------------

// Internal collision object conversion handle
struct CollisionObjHandle
{
	const void* obj;
	SupportFunc support;
	Vqs xform;
};

// Internal support function for all ccd callbacks (will pass in user function through this cb), 
// this way I can keep everything consistent, only expose gs related APIs, wrap any 3rd party libs, 
// and possibly change internals in the future to custom implementations without breaking anything.
void CCDSupportFunc(const void* _obj, const ccd_vec3_t* _dir, ccd_vec3_t* _out);

//-----------------------------------------------------------------------------
// CCD
//-----------------------------------------------------------------------------

#define _is_nan(V) ((V) != (V))

// Useful CCD conversions
inline void _ccdv32gsv3(const ccd_vec3_t* _in, glm::vec3* _out)
{
	// Safe check against NaNs
	if (_is_nan(_in->v[0]) || _is_nan(_in->v[1]) || _is_nan(_in->v[2])) return;
	*_out = glm::vec3((float)_in->v[0], (float)_in->v[1], (float)_in->v[2]);
}

inline void _gsv32ccdv3(const glm::vec3* _in, ccd_vec3_t* _out)
{
	ccdVec3Set(_out, CCD_REAL(_in->x), CCD_REAL(_in->y), CCD_REAL(_in->z));
}

int32_t CCDGJKInternal(const void* c0, const Vqs& xform_a, SupportFunc f0, const void* c1, const Vqs& xform_b, SupportFunc f1, ContactInfo* res)
{
	// Convert to appropriate gjk internals, then call ccd
	ccd_t ccd = {};
	CCD_INIT(&ccd);

	// set up ccd_t struct
	ccd.support1 = CCDSupportFunc;  // support function for first object
	ccd.support2 = CCDSupportFunc;  // support function for second object
	ccd.max_iterations = 100;                   // maximal number of iterations
	ccd.epa_tolerance = 0.0001;                // maximal tolerance for epa to succeed

	// Default transforms
	//Vqs _xa, _xb;

	// Collision object 1
	CollisionObjHandle h0 = {};
	h0.support = f0;
	h0.obj = c0;
	h0.xform = xform_a;

	// Collision object 2
	CollisionObjHandle h1 = {};
	h1.support = f1;
	h1.obj = c1;
	h1.xform = xform_b;

	// Call ccd, cache results into res for user
	ccd_real_t depth = CCD_REAL(0.0);
	ccd_vec3_t n = ccd_vec3_t{ 0.f, 0.f, 0.f }, p = ccd_vec3_t{ 0.f, 0.f, 0.f };
	int32_t r = ccdGJKPenetration(&h0, &h1, &ccd, &depth, &n, &p);
	int32_t hit = r >= 0 && !_is_nan(n.v[0]) && !_is_nan(n.v[1]) && !_is_nan(n.v[2]);

	if (hit && res)
	{
		res->hit = true;
		res->depth = (float)depth;
		_ccdv32gsv3(&p, &res->point);
		_ccdv32gsv3(&n, &res->normal);
	}

	return r;
}

// Internal support function for gs -> ccd
void CCDSupportFunc(const void* _obj, const ccd_vec3_t* _dir, ccd_vec3_t* _out)
{
	const CollisionObjHandle* obj = (const CollisionObjHandle*)_obj;
	if (obj->support)
	{
		// Temp copy conversion for direction vector
		glm::vec3 dir = glm::vec3(0.0f), out = glm::vec3(0.0f);
		_ccdv32gsv3((const ccd_vec3_t*)_dir, &dir);

		// Call user provided support function
		// Think I found it...
		obj->support(obj->obj, obj->xform, dir, &out);

		// Copy over out result for ccd
		_gsv32ccdv3(&out, (ccd_vec3_t*)_out);
	}
}