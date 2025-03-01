/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HK_FLOATING_POINT_EXCEPTION_CHECK_H
#define HK_FLOATING_POINT_EXCEPTION_CHECK_H

#include <Common/Base/hkBase.h>

// Push and set state
// If addToCurrentState is false the current fpu state will be over-riden with the new state
// If addToCurrentState is true the current fpu state will be or'd with the new state
void HK_CALL hkPushFPUState(unsigned int state, hkBool addToCurrentState = true);

// Pop and restore state	
void HK_CALL hkPopFPUState();

#endif // HK_FLOATING_POINT_EXCEPTION_CHECK_H

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
