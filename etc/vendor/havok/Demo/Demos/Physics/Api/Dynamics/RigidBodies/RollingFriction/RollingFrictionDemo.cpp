/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <Demos/demos.h>
#include <Demos/Physics/Api/Dynamics/RigidBodies/RollingFriction/RollingFrictionDemo.h>

#if defined HK_ENABLE_ROLLING_FRICITON_CODE

RollingFrictionDemo::RollingFrictionDemo(hkDemoEnvironment* env)
:	hkDefaultPhysicsDemo(env)
{
	//
	// Setup the camera
	//
	{
		hkVector4 from(0.0f, 7.0f, 20.0f);
		hkVector4 to  (0.0f, 3.0f,  0.0f);
		hkVector4 up  (0.0f, 1.0f,  0.0f);
		setupDefaultCameras( env, from, to, up );
	}


	//
	// Create the world
	//
	{
		hkpWorldCinfo info;
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_4ITERS_MEDIUM); 
		info.setBroadPhaseWorldSize( 100.0f );
		info.m_enableDeactivation = false;
		m_world = new hkpWorld( info );
		m_world->lock();

		setupGraphics();
	}

	//
	// Register the agents
	//
	{
		hkpAgentRegisterUtil::registerAllAgents(m_world->getCollisionDispatcher());
	}


	//
	// Illustrative parameters for the demo
	//

	const int numObjects = 10;					// the number of moving boxes
	const hkReal objectStep = 1.2f;				// a notional 'width' of a moving box pair
	const hkReal initialFriction = 0.0f;
	const hkReal frictionRange = 1.0f;				// friction ranges from initialFriction to frictionRange
	const hkVector4 axis(0.0f,0.0f,1.0f);
	const hkReal angle = 0.0f;//-HK_REAL_PI / 10.0f;
	const hkQuaternion boxOrientation(axis, angle);		// Orientation
	const hkReal fixedBoxHalfHeight = 1.0f;
	const hkReal movingBoxHalfHeight = 0.1f;
	const hkReal movingBoxInitialVerticalDisplacement = 0.25f;


	//
	// Derived parameters for the demo
	//

	const hkReal frictionStep = frictionRange / numObjects;	// friction ranges in steps of frictionStep
	const hkReal positionAlongZ = - (objectStep * numObjects) / 2.0f;
	const hkVector4 fixedBoxSize((objectStep * numObjects) / 2.0f + 2.0f, fixedBoxHalfHeight, (objectStep * numObjects) / 2.0f + 2.0f);
	const hkVector4 boxSize(0.5f, movingBoxHalfHeight, 0.5f);

	// Since we've rotated everything, the height above the fixed box is proportional to the 1/cosine of the angle we rotated by.
	const hkReal movingBoxCentreHeight = (fixedBoxHalfHeight + movingBoxHalfHeight + movingBoxInitialVerticalDisplacement) / hkMath::cos(angle);


	//
	// Box shapes
	//
	hkpBoxShape* fixedBoxShape = new hkpBoxShape( fixedBoxSize , 0 );
	hkpSphereShape* ballShape = new hkpSphereShape( 0.5f );


	//
	// Physical parameters for the demo
	//
	hkReal fixedBoxFriction = 0.25f;
	hkReal boxMass = 1.0f;

	//
	// Create the fixed box
	//
	{
		// Position of the box
		hkVector4 boxPosition(0.0f, 0.0f, 0.0f);

		// Set up the construction info parameters for the box
		hkpRigidBodyCinfo info;
		info.m_motionType = hkpMotion::MOTION_FIXED;

		info.m_friction = fixedBoxFriction;

		info.m_shape = fixedBoxShape;
		info.m_position = boxPosition;
		info.m_rotation = boxOrientation;

		// Create fixed box
		hkpRigidBody* box = new hkpRigidBody(info);
#if defined HK_ENABLE_ROLLING_FRICITON_CODE
		box->getMaterial().setRollingFrictionMultiplier(1.0f);
#endif
		m_world->addEntity(box);
		box->removeReference();
	}

	//
	// Create the moving boxes
	//

	for(int i = 1; i < numObjects; i++)
	{

		// Compute the box inertia tensor
		hkpMassProperties massProperties;
		hkpInertiaTensorComputer::computeBoxVolumeMassProperties(boxSize, boxMass, massProperties);

		// Set up the construction info parameters for the box
		hkpRigidBodyCinfo info;
			
		// Each box has a different friction ranging from 0.0 to 0.9
		info.m_friction = initialFriction + frictionStep * i;	

		info.m_shape = ballShape;
		info.m_mass  = boxMass;
		info.m_inertiaTensor = massProperties.m_inertiaTensor;
		info.m_motionType = hkpMotion::MOTION_BOX_INERTIA;
		info.m_linearVelocity.set(5.0f, 0.0f, 0.0f);

		// Position of the box
		float zpos = positionAlongZ + objectStep * i;

		info.m_position = hkVector4(-5.0f, movingBoxCentreHeight, zpos);
		info.m_rotation = boxOrientation;

		// Create a box
		hkpRigidBody* box = new hkpRigidBody(info);
#if defined HK_ENABLE_ROLLING_FRICITON_CODE
		box->getMaterial().setRollingFrictionMultiplier(0.05f);
#endif
		m_world->addEntity(box);
		box->removeReference();

	}

	/// Once we've created the bodies and added them to the world we simply let the simulation proceed.

	//
	// Remove references from shapes
	//
	fixedBoxShape->removeReference();
	ballShape->removeReference();

	m_world->unlock();
}


#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#	pragma fullpath_file on
#endif

static const char helpString[] = \
"A demo which shows how to set the rolling friction property of a rigid body.";

HK_DECLARE_DEMO(RollingFrictionDemo, HK_DEMO_TYPE_PRIME, "Varying friction values, from slippy to sticky", helpString);

#endif

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
