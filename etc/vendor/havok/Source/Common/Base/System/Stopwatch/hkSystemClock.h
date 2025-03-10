/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#ifndef HKBASE_HKSYSTEMCLOCK_H
#define HKBASE_HKSYSTEMCLOCK_H

/// An interface to hardware timers.
#if !defined(HK_PLATFORM_PS3_SPU)

class hkSystemClock : public hkReferencedObject, public hkSingleton<hkSystemClock>
{
	public:

			/// Get the current tick counter.
		virtual hkUint64 getTickCounter() = 0;

			/// Return how many ticks elapse per second.
		virtual hkUint64 getTicksPerSecond() = 0;
};

#else

class hkSystemClock 
{
public:

	/// Get the current tick counter.
	static hkUint64 getTickCounter();

	/// Return how many ticks elapse per second.
	static hkUint64 getTicksPerSecond();
};

#endif

#endif //HKBASE_HKSYSTEMCLOCK_H

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
