/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_GravityChangeDemo_H
#define HK_GravityChangeDemo_H

#include <Demos/DemoCommon/DemoFramework/hkDefaultPhysicsDemo.h>

/// A demo which shows how to set the GravityChange property of a rigid body.
/// 10 spheres of GravityChange ranging 0 - 1 are dropped onto 10 fixed boxes.
/// NOTE: The current implementation of GravityChange uses a very simple approximation, and
/// may not produce physically accurate results. A more accurate version will 
/// implemented in later versions of the SDK.
class GravityChangeDemo : public hkDefaultPhysicsDemo
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_DEMO);

		GravityChangeDemo(hkDemoEnvironment* env);
};

#endif	// HK_GravityChangeDemo_H

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20090704)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/
