/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

#include <Demos/demos.h>
#include <Demos/Animation/Api/MotionExtraction/MotionExtraction/MotionExtractionDemo.h>

#include <Common/Base/Reflection/hkClass.h>
#include <Common/Base/Container/LocalArray/hkLocalArray.h>

// Serialization
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Base/Reflection/Registry/hkTypeInfoRegistry.h>

// Asset mgt
#include <Demos/DemoCommon/Utilities/Asset/hkAssetManagementUtil.h>

// Skeletal Animation
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/Rig/hkaSkeletonUtils.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Animation/hkaAnimationBinding.h>

// Vertex Deformation
#include <Animation/Animation/Deform/Skinning/hkaMeshBinding.h>

// Scene Data
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/SceneData/Mesh/hkxMesh.h>
#include <Animation/Animation/Rig/hkaPose.h>

// Common animation Utilities
#include <Demos/DemoCommon/Utilities/Animation/AnimationUtils.h>

// Graphics Stuff
#include <Demos/DemoCommon/DemoFramework/hkTextDisplay.h>
#include <Graphics/Bridge/SceneData/hkgSceneDataConverter.h>

// Graphics Stuff
#include <Graphics/Common/Window/hkgWindow.h>
#include <Common/Visualize/hkDebugDisplay.h>



MotionExtractionDemo::MotionExtractionDemo( hkDemoEnvironment* env )
:	hkDefaultAnimationDemo(env)
{

	// Disable warnings: if no renderer									
	if( hkString::strCmp( m_env->m_options->m_renderer, "n" ) == 0 )
	{
		hkError::getInstance().setEnabled(0xf0d1e423, false); //'Could not realize an inplace texture of type PNG.'
	}

	// want to do software skinning always in this demo
	// see HardwareSkinningDemo for how to setup hardware palettes etc
	m_env->m_sceneConverter->setAllowHardwareSkinning(false);

	//
	// Setup the camera
	//
	{
		hkVector4 from( 3,3,1 );
		hkVector4 to  ( 0,0,0 );
		hkVector4 up  ( 0.0f, 0.0f, 1.0f );
		setupDefaultCameras( env, from, to, up, 0.1f, 100 );

		// so we can use the sticks on consoles
		if (m_env->m_options->m_trackballMode == 0)
		{
			m_forcedOnTrackball = true;
			m_env->m_window->getViewport(0)->setNavigationMode(HKG_CAMERA_NAV_TRACKBALL);
		}
	}

	m_loader = new hkLoader();

	// Convert the scene
	{
		hkString assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/Scene/hkScene.hkx");
		hkRootLevelContainer* container = m_loader->load( assetFile.cString() );
		HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");
		hkxScene* scene = reinterpret_cast<hkxScene*>( container->findObjectByType( hkxSceneClass.getName() ));

		HK_ASSERT2(0x27343635, scene, "No scene loaded");
		removeLights(m_env);
		env->m_sceneConverter->convert( scene );
	}

	// Get the rig
	{
		hkString assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkRig.hkx");
		hkRootLevelContainer* container = m_loader->load( assetFile.cString() );
		HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");
		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

		HK_ASSERT2(0x27343435, ac && (ac->m_numSkeletons > 0), "No skeleton loaded");
		m_skeleton = ac->m_skeletons[0];
	}

	// Get the animation and the binding
	{
		hkString assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkWalkLoop.hkx");
		hkRootLevelContainer* container = m_loader->load( assetFile.cString() );
		HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");
		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

		HK_ASSERT2(0x27343435, ac && (ac->m_numAnimations > 0 ), "No animation loaded");
		m_animation[0] =  ac->m_animations[0];

		HK_ASSERT2(0x27343435, ac && (ac->m_numBindings > 0), "No binding loaded");
		m_binding[0] = ac->m_bindings[0];

		assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkWalkTurnLLoop.hkx");
		container = m_loader->load( assetFile.cString() );
		ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

		HK_ASSERT2(0x27343435, ac && (ac->m_numAnimations > 0 ), "No animation loaded");
		m_animation[1] =  ac->m_animations[0];

		HK_ASSERT2(0x27343435, ac && (ac->m_numBindings > 0), "No binding loaded");
		m_binding[1] = ac->m_bindings[0];

		assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkWalkTurnRLoop.hkx");
		container = m_loader->load( assetFile.cString() );
		ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));

		HK_ASSERT2(0x27343435, ac && (ac->m_numAnimations > 0 ), "No animation loaded");
		m_animation[2] =  ac->m_animations[0];

		HK_ASSERT2(0x27343435, ac && (ac->m_numBindings > 0), "No binding loaded");
		m_binding[2] = ac->m_bindings[0];
	}

	// Convert the skin
	{
		const char* skinFile = "Resources/Animation/HavokGirl/hkLowResSkin.hkx";
		hkString assetFile = hkAssetManagementUtil::getFilePath( skinFile );
		hkRootLevelContainer* container = m_loader->load( assetFile.cString() );
		HK_ASSERT2(0x27343437, container != HK_NULL , "Could not load asset");

		hkxScene* scene = reinterpret_cast<hkxScene*>( container->findObjectByType( hkxSceneClass.getName() ));
		HK_ASSERT2(0x27343435, scene , "No scene loaded");

		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));
		HK_ASSERT2(0x27343435, ac && (ac->m_numSkins > 0), "No skins loaded");

		m_numSkinBindings = ac->m_numSkins;
		m_skinBindings = ac->m_skins;

		// Make graphics output buffers for the skins
		env->m_sceneConverter->convert( scene );
	}

	// Create the skeleton
	m_skeletonInstance = new hkaAnimatedSkeleton( m_skeleton );
	m_skeletonInstance->setReferencePoseWeightThreshold( 0.01f );

	float weights[NUM_ANIMS];
	weights[0] = 0.1f;
	weights[1] = 0.0f;
	weights[2] = 0.0f;

	// Grab the animations
	for (int i=0; i < NUM_ANIMS; i++)
	{
		m_control[i] = new hkaDefaultAnimationControl( m_binding[i] );
		m_control[i]->setMasterWeight( weights[i] );
		m_control[i]->setPlaybackSpeed( 1.0f );

		m_skeletonInstance->addAnimationControl( m_control[i] );
		m_control[i]->removeReference();
	}

	// make a world so that we can auto create a display world to hold the skin
	setupGraphics( );

	m_currentMotion.setIdentity();
}

MotionExtractionDemo::~MotionExtractionDemo()
{
	// Re-enable warnings									
	hkError::getInstance().setEnabled(0xf0d1e423, true); 

	m_skeletonInstance->removeReference();
	delete m_loader;

	if (m_forcedOnTrackball)
		m_env->m_window->getViewport(0)->setNavigationMode(HKG_CAMERA_NAV_FLY);
}

hkDemo::Result MotionExtractionDemo::stepDemo()
{
	static hkReal val = 0.0f;

	hkReal stickX = m_env->m_gamePad->getStickPosX(1);
	val += (stickX - val) * 0.3f;

	hkReal leftWeight = (val < 0) ? -val : 0;
	hkReal rightWeight = (val > 0) ? val : 0;

	m_control[1]->setMasterWeight( leftWeight );
	m_control[2]->setMasterWeight( rightWeight );

	// Grab accumulated motion
	{
		hkQsTransform deltaMotion;
		deltaMotion.setIdentity();
		m_skeletonInstance->getDeltaReferenceFrame( m_timestep, deltaMotion);
		m_currentMotion.setMulEq(deltaMotion);
	}

	// Show the extracted motion for the next 2 seconds

	const hkReal freq = 60;
	hkVector4 lp = m_currentMotion.getTranslation();

	for (hkReal tm = 0.0f; tm < 2.0f; tm += 1.0f / freq)
	{
		hkQsTransform deltaMotion;
		deltaMotion.setIdentity();
		m_skeletonInstance->getDeltaReferenceFrame(tm, deltaMotion);
		hkVector4 p;
		p.setTransformedPos(m_currentMotion, deltaMotion.getTranslation());

		HK_DISPLAY_LINE( lp, p, 0xffff0f0f);
		lp = p;
	}

	// Advance the active animations
	m_skeletonInstance->stepDeltaTime( m_timestep );

	hkaPose pose (m_skeleton);

	// Sample the active animations and combine into a single pose
	m_skeletonInstance->sampleAndCombineAnimations( pose.accessUnsyncedPoseLocalSpace().begin(), pose.getFloatSlotValues().begin()  );
	AnimationUtils::drawPose( pose, hkQsTransform::getIdentity() );

	// Skin
	{
		// Work with the pose in Model space
		const hkArray<hkQsTransform>& poseInWorld = pose.getSyncedPoseModelSpace();

		// Construct the composite world transform
		const int boneCount = m_skeleton->m_numBones;
		hkLocalArray<hkTransform> compositeWorldInverse( boneCount );
		compositeWorldInverse.setSize( boneCount );

		// Convert accumlated info to graphics matrix
		hkTransform graphicsTransform;
		m_currentMotion.copyToTransform(graphicsTransform);

		// Skin the meshes
		for (int i=0; i < m_numSkinBindings; i++)
		{
			// assumes either a straight map (null map) or a single one (1 palette)
			hkInt16* usedBones = m_skinBindings[i]->m_mappings? m_skinBindings[i]->m_mappings[0].m_mapping : HK_NULL;
			int numUsedBones = usedBones? m_skinBindings[i]->m_mappings[0].m_numMapping : boneCount;

			// Multiply through by the bind pose inverse world inverse matrices
			for (int p=0; p < numUsedBones; p++)
			{
				int boneIndex = usedBones? usedBones[p] : p;
				compositeWorldInverse[p].setMul( poseInWorld[ boneIndex ], m_skinBindings[i]->m_boneFromSkinMeshTransforms[ boneIndex ] );
			}

			AnimationUtils::skinMesh( *m_skinBindings[i]->m_mesh, graphicsTransform, compositeWorldInverse.begin(), *m_env->m_sceneConverter );
		}
	}

	return hkDemo::DEMO_OK;
}

#if defined(HK_COMPILER_MWERKS)
#	pragma force_active on
#	pragma fullpath_file on
#endif

static const char helpString[] = \
"3 animations, blended togther. All animations have their motion extracted once loaded. This extracted motion is accumulated as the demo runs.\n" \
"Use the dPad to control the weight of each animation. \221 resets the characters position to the origin.\n";

HK_DECLARE_DEMO(MotionExtractionDemo, HK_DEMO_TYPE_ANIMATION | HK_DEMO_TYPE_SERIALIZE, "Motion Extraction", "Motion extraction and accumulation");

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
