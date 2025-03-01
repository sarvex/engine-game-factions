/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */


#include <Demos/demos.h>

#include <Demos/DemoCommon/Utilities/Character/DemoCharacter/AnimatedDemoCharacter/AnimatedDemoCharacter.h>
#include <Demos/DemoCommon/Utilities/Character/CharacterStepInput.h>
#include <Demos/DemoCommon/Utilities/Character/CharacterProxy/CharacterProxy.h>
#include <Demos/DemoCommon/Utilities/Character/CharacterUtils.h>

#include <Common/Visualize/hkDebugDisplay.h>
#include <Graphics/Bridge/SceneData/hkgSceneDataConverter.h>
#include <Graphics/Bridge/SceneData/hkgVertexBufferWrapper.h>

// Serialization
#include <Common/Base/Reflection/hkClass.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Base/Reflection/Registry/hkTypeInfoRegistry.h>
#include <Common/Serialize/Util/hkBuiltinTypeRegistry.h>
#include <Common/Base/Container/LocalArray/hkLocalBuffer.h>

// Utilities
#include <Demos/DemoCommon/Utilities/Asset/hkAssetManagementUtil.h>
#include <Demos/DemoCommon/Utilities/Animation/AnimationUtils.h>

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
#include <Common/SceneData/Mesh/hkxMeshSection.h>
#include <Common/SceneData/Mesh/hkxIndexBuffer.h>
#include <Common/SceneData/Mesh/hkxVertexBuffer.h>
#include <Animation/Animation/Rig/hkaPose.h>

// State Machine
#include <Demos/DemoCommon/Utilities/Character/DemoCharacter/AnimatedDemoCharacter/StateMachine/SimpleBipedStateMachine.h>
#include <Demos/DemoCommon/Utilities/Character/DemoCharacter/AnimatedDemoCharacter/StateMachine/States/SimpleBipedStates.h>

// See what renderer we are using etc
#include <Graphics/Bridge/System/hkgSystem.h> // to figure put if we should hardware skin
#include <Graphics/Common/Shader/hkgShader.h>
#include <Graphics/Common/Shader/hkgShaderCollection.h>
#include <Graphics/Common/DisplayObject/hkgDisplayObject.h>
#include <Graphics/Common/Geometry/hkgGeometry.h>
#include <Graphics/Common/Geometry/hkgMaterialFaceSet.h>
#include <Graphics/Common/Geometry/VertexSet/hkgVertexSet.h>
#include <Graphics/Common/DisplayWorld/hkgDisplayWorld.h>

#include <Demos/DemoCommon/Utilities/Character/CharacterProxy/RigidBodyCharacterProxy/RigidBodyCharacterProxy.h>
#include <Physics/Utilities/CharacterControl/CharacterRigidBody/hkpCharacterRigidBody.h>

namespace
{
	hkReal FORWARD_VELOCITY_SCALAR = 1.0f;

	// debug visualization
	bool DRAW_PROXY = false;
}

AnimatedCharacterFactory::AnimatedCharacterFactory( CharacterType defaultType )
{
	m_type = defaultType;
	m_loader = new hkLoader();

	for (int i = 0; i < MAX_CHARACTER_TYPE; ++i)
	{
		m_animSet[i].m_skeleton = HK_NULL;
		m_display[i].m_numSkinBindings = 0;
		m_display[i].m_skinBindings = HK_NULL;
	}

	loadBasicAnimations( m_type );
}

AnimatedCharacterFactory::~AnimatedCharacterFactory()
{
	m_loader->removeReference();
}

DemoCharacter* AnimatedCharacterFactory::createCharacterUsingProxy(	CharacterProxy* proxy,
																	const hkVector4& gravity,
																	hkDemoEnvironment* env,
																	CharacterType characterType )
{
	if ( !DRAW_PROXY )
	{
		proxy->getWorldObject()->removeProperty(HK_PROPERTY_DEBUG_DISPLAY_COLOR);
		proxy->getWorldObject()->addProperty(HK_PROPERTY_DEBUG_DISPLAY_COLOR, 0x00FFFFFF);
	}

	// Animated Character
	AnimatedDemoCharacterCinfo info;
	info.m_characterProxy = proxy;
	info.m_gravity = gravity;
	info.m_animationForwardLocal = m_animSet[m_type].m_animFwdLocal;
	info.m_animationUpLocal = m_animSet[m_type].m_animUpLocal;
	info.m_animationSet = &m_animSet[m_type];

	AnimatedDemoCharacter* animCharacter = new AnimatedDemoCharacter( info );
	
	if (m_display[m_type].m_numSkinBindings > 0)	
	{
		animCharacter->cloneSkin( env, m_display[m_type] );
	}
	else
	{
		animCharacter->loadSkin( m_loader, env, m_type, m_display[m_type] );
	}
	
	return animCharacter;	
}



void AnimatedCharacterFactory::loadBasicAnimations( CharacterType type )
{
	hkString rigFile;
	char* refBone = HK_NULL;
	char* rootBone = HK_NULL;

	AnimatedDemoCharacterAnimationSet* set = &m_animSet[type];

	set->m_walkRunSyncOffset = 0.0f;


	switch (type)
	{
		case CHARACTER_TYPE_HAVOK_GIRL:
			{
				rigFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkRig.hkx");
				refBone = "reference";
				rootBone = "root";

				// Load the animations
				{
					set->m_idle	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkIdle.hkx" );
					set->m_jump	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkJumpLandLoop.hkx" );
					set->m_inAir = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkProtect.hkx" );
					set->m_land	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkProtect.hkx" );
					set->m_walk	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkWalkLoop.hkx");
					set->m_run	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkRunLoop.hkx");
					set->m_dive	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/HavokGirl/hkScale.hkx" );
				}

				set->m_animFwdLocal.set(1,0,0);
				set->m_animUpLocal.set(0,0,1);
			}
			break;
		case CHARACTER_TYPE_FIREFIGHTER:
		case CHARACTER_TYPE_JUNGLE_FIGHTER:
		case CHARACTER_TYPE_DESERT_FIGHTER:
		case CHARACTER_TYPE_SNOW_FIGHTER:
			{
				rigFile = hkAssetManagementUtil::getFilePath("Resources/Animation/ShowCase/Gdc2005/Model/Firefighter_Rig.hkx");
				refBone = "reference";
				rootBone = "root";
				set->m_walkRunSyncOffset = 17.0f / 60.0f;

				// Load the animations
				{
					set->m_idle	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkIdle1.hkx" );
					set->m_jump	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkRunJump.hkx" );
					set->m_inAir = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkInAir.hkx" );
					set->m_land	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkHardLand.hkx" );
					set->m_walk	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkWalk.hkx");
					set->m_run	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkRun.hkx");
					set->m_dive	 = AnimationUtils::loadAnimation( *m_loader, "Resources/Animation/ShowCase/Gdc2005/Animations/hkDive.hkx" );
				}

				set->m_animFwdLocal.set(0,0,1);
				set->m_animUpLocal.set(0,1,0);
			}
			break;
		default:
			{
				HK_ASSERT2(0x0, 0, "Invalid character type");
			}
	}

	// Get the rig
	{

		hkError::getInstance().setEnabled(0x9fe65234, false); // "Unsupported simulation type..."
		hkError::getInstance().setEnabled(0x651f7aa5, false); // "removing deprecated object of type..."
		set->m_rigContainer = m_loader->load( rigFile.cString() );
		HK_ASSERT2(0x27343437, set->m_rigContainer != HK_NULL , "Could not load asset");
		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( set->m_rigContainer->findObjectByType( hkaAnimationContainerClass.getName() ));
		HK_ASSERT2(0x27343435, ac && (ac->m_numSkeletons > 0), "No skeleton loaded");
		set->m_skeleton = ac->m_skeletons[0];
		hkError::getInstance().setEnabled(0x651f7aa5, true);
		hkError::getInstance().setEnabled(0x9fe65234, true);

	}


	// Add constraints to skeleton
	{
		const hkaSkeleton* skeleton = set->m_skeleton;

		// Lock translations (except root, named "position")
		hkaSkeletonUtils::lockTranslations(*skeleton);

		// and except also the children "reference" and "root"
		const hkInt16 referenceBoneIdx = hkaSkeletonUtils::findBoneWithName(*skeleton, refBone );
		const hkInt16 rootBoneIdx = hkaSkeletonUtils::findBoneWithName(*skeleton, rootBone);
		if (referenceBoneIdx != -1)
			skeleton->m_bones[referenceBoneIdx]->m_lockTranslation = false;
		if (rootBoneIdx != -1)
			skeleton->m_bones[rootBoneIdx]->m_lockTranslation = false;
	}


}


void computeProxyFromAnimation(	const hkVector4& animUp,
								const hkVector4& animForward,
								const hkVector4& proxyUp,
								const hkVector4& proxyForward,
								hkRotation& proxyFromAnimation )
{
	hkVector4 right; right.setCross( animForward, animUp );
	hkRotation animRot;	animRot.setCols( animUp, animForward, right );
	animRot.transpose();

	HK_ASSERT( 0, proxyUp.dot3(proxyForward) == 0 );

	hkVector4 proxyRight; proxyRight.setCross( proxyForward, proxyUp );
	hkRotation proxyRot; proxyRot.setCols( proxyUp, proxyForward, proxyRight );

	proxyFromAnimation.setMul( proxyRot, animRot );
}

AnimatedDemoCharacter::AnimatedDemoCharacter( AnimatedDemoCharacterCinfo& info )
: DemoCharacter(info)
{
	m_gravity = info.m_gravity;

	//
	// Setup the m_characterFromAnimation transform
	//
	m_animationUpLocal = info.m_animationUpLocal;
	m_animationForwardLocal = info.m_animationForwardLocal;

	computeProxyFromAnimation(	m_animationUpLocal,
								m_animationForwardLocal,
								m_characterProxy->getUpLocal(),
								m_characterProxy->getForwardLocal(),
								m_characterFromAnimation );

	m_skinsLoaded = false;
	m_hardwareSkinning = false;

	initAnimation( info.m_animationSet );
}

AnimatedDemoCharacter::~AnimatedDemoCharacter()
{	
	m_animationStateMachine->removeReference();
	m_animationMachine->removeReference();
}

const hkaSkeleton* AnimatedDemoCharacter::getSkeleton() const
{ 
	return m_animatedSkeleton->getSkeleton(); 
}

bool AnimatedDemoCharacter_supportsHardwareSkinning(hkDemoEnvironment* env)
{

#if defined( HK_PLATFORM_PS2 ) || defined( HK_PLATFORM_XBOX360 ) || defined( HK_PLATFORM_PS3_PPU ) 

	return true;

#elif defined (HK_PLATFORM_WIN32)

	// Only reason not supported in DX10 is just because I haven't written the hkgBlendMatrixSet for them, which is easy if required.
	bool hardwareSkinning = (hkgSystem::g_RendererType != hkgSystem::HKG_RENDERER_NULL);
	if (hardwareSkinning )
	{
		// check if it has enough shader support to run. We will assume we require 2.0 (so get 256 vshader constants to do fast skinning), not unreasonable, may run on 1.1
		hardwareSkinning = env->m_window->vertexShaderMajorVersion() >= 2; 
	}
	return hardwareSkinning;

#else 
	
	// Xbox(can, but have to load compiled shaders, see the h/w skinning demo, can add here if required)
	// GCN (no, could do like PSP(R) (PlayStation(R)Portable)?)
	// WII (no, could do like PSP(R)?) 
	// OglMac, OglLinux, (again just because no Cg shader funcs setup yet, very easy)
	return false; 

#endif

}

hkgShaderCollection* AnimatedDemoCharacter_loadSkinningShader(hkDemoEnvironment* env)
{
	hkBool shouldCompileShaders = env->m_window->supportsShaderCompilation();

	hkgShaderCollection* ret = HK_NULL;
	if (shouldCompileShaders)
	{
		hkString shaderFile = "./Resources/Animation/Shaders/SimpleSkinningShader"; 
		
		hkgDisplayContext* ctx = env->m_window->getContext();

		hkgShader* vertexShader = hkgShader::createVertexShader( ctx );
		hkgShader* pixelShader = hkgShader::createPixelShader( ctx );
		hkgShader* vertexShaderS0 = HK_NULL; 
		hkgShader* pixelShaderS0 = HK_NULL;
		hkgShader* vertexShaderS1 = HK_NULL; 
		hkgShader* pixelShaderS1 = HK_NULL; 

		shaderFile += hkString(vertexShader->getDefaultFileNameExtension());

		hkArray<const char*> defines;
		env->m_window->buildCommonShaderDefines( defines);

		HKG_SHADER_RENDER_STYLE basicStyle = HKG_SHADER_RENDER_1LIGHTS | HKG_SHADER_RENDER_MODULATE_TEXTURE0 | HKG_SHADER_RENDER_BLENDING;
		vertexShader->realizeCompileFromFile( shaderFile.cString(), "mainVS", basicStyle, HK_NULL, defines.begin());
		pixelShader->realizeCompileFromFile( shaderFile.cString(), "mainPS",basicStyle, HK_NULL, defines.begin());

		if ( env->m_window->getShadowMapSupport() > HKG_SHADOWMAP_NOSUPPORT ) 
		{
			vertexShaderS0 = hkgShader::createVertexShader( ctx );
			pixelShaderS0 = hkgShader::createPixelShader( ctx );
			vertexShaderS1 = hkgShader::createVertexShader( ctx );
			pixelShaderS1 = hkgShader::createPixelShader( ctx );

			HKG_SHADER_RENDER_STYLE toDepthStyle;
			HKG_SHADER_RENDER_STYLE withDepthStyle;
			env->m_window->getShadowMapPassStyles(toDepthStyle, withDepthStyle);

			vertexShaderS0->realizeCompileFromFile( shaderFile.cString(), "mainVS_ToDepth", toDepthStyle | basicStyle , HK_NULL, defines.begin());
			pixelShaderS0->realizeCompileFromFile( shaderFile.cString(), "mainPS_ToDepth", toDepthStyle | basicStyle, HK_NULL, defines.begin());

			vertexShaderS1->realizeCompileFromFile( shaderFile.cString(), "mainVS_Shadow", withDepthStyle | basicStyle, HK_NULL, defines.begin());
			pixelShaderS1->realizeCompileFromFile( shaderFile.cString(), "mainPS_Shadow", withDepthStyle | basicStyle, HK_NULL, defines.begin());
		}

		ret = hkgShaderCollection::create();
		ret->addShaderGrouping(vertexShader, pixelShader);	
		pixelShader->removeReference();
		vertexShader->removeReference();
		
		if (vertexShaderS0)
		{
			ret->addShaderGrouping(vertexShaderS0, pixelShaderS0);	
			ret->addShaderGrouping(vertexShaderS1, pixelShaderS1);	
			pixelShaderS0->removeReference();
			vertexShaderS0->removeReference();
			pixelShaderS1->removeReference();
			vertexShaderS1->removeReference();
		}
	}
	return ret;
}

void AnimatedDemoCharacter_setSkinningShader( hkgShaderCollection* shader, hkgDisplayObject* skin)
{
	for (int g=0; g < skin->getNumGeometry(); ++g)
	{
		hkgGeometry* geom = skin->getGeometry(g);
		for (int m=0; geom && m < geom->getNumMaterialFaceSets(); ++m)
		{
			hkgMaterial* mat = geom->getMaterialFaceSet(m)->getMaterial();
			if (mat)
			{
				mat->setShaderCollection( shader );
			}
		}
	}
}

void convertSkin(	hkaMeshBinding** skinBindings,
					hkInt32 numSkinBindings,
					hkxScene* scene,
					hkDemoEnvironment* env,
					bool hardwareSkinning,
					const char* texturePath,
					hkArray<hkgDisplayObject*>& displayObjectsOut )
{
	int numPrevSkins = env->m_sceneConverter->m_addedSkins.getSize();
	
	if (texturePath && !env->m_sceneConverter->hasTextureSearchPath(texturePath))
	{
		env->m_sceneConverter->addTextureSearchPath(texturePath);
	}

	// Make graphics output buffers for the skins
	env->m_sceneConverter->setAllowHardwareSkinning( hardwareSkinning ); // will enable the added blend info in the vertex buffers
	env->m_sceneConverter->convert( scene );
		
	if (hardwareSkinning)
	{
		for (int ms=0; ms < numSkinBindings; ++ms)
		{
			hkaMeshBinding* skinBinding = skinBindings[ms];
			if ( !env->m_sceneConverter->setupHardwareSkin( env->m_window->getContext(), skinBinding->m_mesh,
				reinterpret_cast<hkgAssetConverter::IndexMapping*>( skinBinding->m_mappings ),
				skinBinding->m_numMappings, (hkInt16)skinBinding->m_skeleton->m_numBones ) ) 
			{
				HK_WARN_ALWAYS( 0x4327defe, "Could not setup the blend matrices for a skin. Will not be able to skin in h/w.");
			}
		}

		int numSkins = env->m_sceneConverter->m_addedSkins.getSize();

		if (hardwareSkinning && (numPrevSkins < numSkins) )
		{
			hkgShaderCollection* shaderSet = AnimatedDemoCharacter_loadSkinningShader(env);
			for (int s=numPrevSkins; hardwareSkinning && (s < numSkins); ++s )
			{
				AnimatedDemoCharacter_setSkinningShader(shaderSet, env->m_sceneConverter->m_addedSkins[s]->m_skin);
			}
			if (shaderSet)
			{
				shaderSet->removeReference();
			}
		}

	}

	for (int ms=0; ms < numSkinBindings; ++ms)
	{
		hkaMeshBinding* skinBinding = skinBindings[ms];
		int mindex = hkgAssetConverter::findFirstMapping( env->m_sceneConverter->m_meshes, skinBinding->m_mesh); 
		if (mindex >= 0) 
		{
			hkgDisplayObject* dispObj = (hkgDisplayObject*)( env->m_sceneConverter->m_meshes[mindex].m_hkgObject );
			displayObjectsOut.pushBack( dispObj );
		}
	}
}

void AnimatedDemoCharacter::loadSkin( hkLoader* m_loader, hkDemoEnvironment* env, AnimatedCharacterFactory::CharacterType type, AnimatedDemoCharacterDisplay& displayInfoCache )
{
	m_hardwareSkinning = AnimatedDemoCharacter_supportsHardwareSkinning(env);

	hkString assetFile;
	const char* texturePath = HK_NULL;
	switch ( type )
	{
	case CharacterFactory::CHARACTER_TYPE_HAVOK_GIRL:
			assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/HavokGirl/hkLowResSkinWithEyes.hkx");
			m_hardwareSkinning = false;
		break;
	case CharacterFactory::CHARACTER_TYPE_FIREFIGHTER:
		{
			assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/ShowCase/Gdc2005/Model/Firefighter_Skin.hkx"); 
			texturePath = "Resources/Animation/ShowCase/Gdc2005/Model";
		}
		break;
	case CharacterFactory::CHARACTER_TYPE_JUNGLE_FIGHTER:
		{
			assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/ShowCase/Gdc2005/Model/JungleJoe_Skin.hkx"); 
			texturePath = "Resources/Animation/ShowCase/Gdc2005/Model";
		}
		break;
	case CharacterFactory::CHARACTER_TYPE_DESERT_FIGHTER:
		{
			assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/ShowCase/Gdc2005/Model/DesertDan_Skin.hkx"); 
			texturePath = "Resources/Animation/ShowCase/Gdc2005/Model";
		}
		break;
	case CharacterFactory::CHARACTER_TYPE_SNOW_FIGHTER:
		{
			assetFile = hkAssetManagementUtil::getFilePath("Resources/Animation/ShowCase/Gdc2005/Model/SnowSam_Skin.hkx"); 
			texturePath = "Resources/Animation/ShowCase/Gdc2005/Model";
		}
		break;
	default:
		{
			HK_ASSERT2(0x0, 0, "Invalid character type");
		}
		break;
	}

	hkRootLevelContainer* rootContainer = m_loader->load( assetFile.cString() );
	HK_ASSERT2(0x27343437, rootContainer != HK_NULL , "Could not load asset");
	hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( rootContainer->findObjectByType( hkaAnimationContainerClass.getName() ));

	HK_ASSERT2(0x27343435, ac && (ac->m_numSkins > 0), "No skins loaded");

	m_numSkinBindings = ac->m_numSkins;
	m_skinBindings = ac->m_skins;

	displayInfoCache.m_hardwareSkinning = m_hardwareSkinning;
	displayInfoCache.m_numSkinBindings = m_numSkinBindings;
	displayInfoCache.m_skinBindings = m_skinBindings;

	hkxScene* scene = reinterpret_cast<hkxScene*>( rootContainer->findObjectByType( hkxSceneClass.getName() ));
	HK_ASSERT2(0x27343435, scene , "No scene loaded");

	convertSkin(	m_skinBindings,
					m_numSkinBindings,
					scene,
					env,
					m_hardwareSkinning,
					texturePath,
					displayInfoCache.m_skinDisplay );

	// use the loaded display objects for this (the first) character
	m_skins = displayInfoCache.m_skinDisplay;

	m_skinsLoaded = true;
}

void cloneSkinDisplayObjects(	hkDemoEnvironment* env,
								AnimatedDemoCharacterDisplay& displayIn,
								hkArray<hkgDisplayObject*>& displayObjectsOut )
{
	hkgSceneDataConverter* sc = env->m_sceneConverter;
	hkgDisplayContext* context = sc->m_context;
	
	if ( displayIn.m_hardwareSkinning )
	{
		for (int ms=0; ms < displayIn.m_numSkinBindings; ++ms)
		{
			hkaMeshBinding* skinBinding = displayIn.m_skinBindings[ms];
			hkgDisplayObject* origDisplayObject = displayIn.m_skinDisplay[ms];

			// XX Once merged with the Head, this will be faster with the Instanced HW Skin object
			// For now just clone the meshes as the blend matrices can't be shared in the face sets
			hkgDisplayObject* newDisplayObject = origDisplayObject->copy( HKG_DISPLAY_OBJECT_SHARE_MATERIALS, context );

			bool allowCulling = false;
			
			hkgAssetConverter::SkinPaletteMap* smap = hkgAssetConverter::setupHardwareSkin(context, 
				newDisplayObject, 
				reinterpret_cast<hkgAssetConverter::IndexMapping*>( skinBinding->m_mappings ), 
				skinBinding->m_numMappings, 
				(hkInt16)skinBinding->m_skeleton->m_numBones,
				allowCulling  );

			if (smap)
			{
				env->m_sceneConverter->m_addedSkins.pushBack( smap );
			}

			env->m_displayWorld->addDisplayObject( newDisplayObject );
			displayObjectsOut.pushBack( newDisplayObject );
			newDisplayObject->removeReference();
		}
	}
	else
	{
		for (int ms=0; ms < displayIn.m_numSkinBindings; ++ms)
		{
			hkgDisplayObject* origDisplayObject = displayIn.m_skinDisplay[ms];
			hkxMesh* origMesh = displayIn.m_skinBindings[ms]->m_mesh;

			// can't copy vertex buffers as they are cpu skinned into so can't instance them
			hkgDisplayObject* newDisplayObject = origDisplayObject->copy( HKG_DISPLAY_OBJECT_SHARE_MATERIALS, context );

			int curSection = 0;
			for (int gi=0; gi < newDisplayObject->getNumGeometry(); ++gi)
			{
				hkgGeometry* geom = newDisplayObject->getGeometry(gi);
				for (int mi=0; mi < geom->getNumMaterialFaceSets(); ++mi)
				{
					for (int fsi=0; fsi < geom->getMaterialFaceSet(mi)->getNumFaceSets(); ++fsi)
					{
						hkgFaceSet* faceSet = geom->getMaterialFaceSet(mi)->getFaceSet(fsi);
						hkgVertexSet* faceVerts = faceSet->getVertexSet();
						
						hkgVertexBufferWrapper* buf = new hkgVertexBufferWrapper(HKG_LOCK_WRITEONLY);
						buf->setVertexDataPtr( (void*)0x1, faceVerts->getNumVerts() );
						buf->setVertexDesc( hkgAssetConverter::constructVertexDesc(faceVerts) );
						buf->setFaceSet(faceSet);
						buf->setTopLevelParent(newDisplayObject);
						hkgAssetConverter::Mapping* mMap = &sc->m_vertexBuffers.expandOne();
						mMap->m_hkgObject = buf;
						mMap->m_origObject = curSection < origMesh->m_numSections? origMesh->m_sections[curSection]->m_vertexBuffer : HK_NULL ;

						++curSection;
					}
				}
			}

			env->m_displayWorld->addDisplayObject( newDisplayObject );
			displayObjectsOut.pushBack( newDisplayObject );
			newDisplayObject->removeReference();
		}
	}
}

void AnimatedDemoCharacter::cloneSkin( hkDemoEnvironment* env, AnimatedDemoCharacterDisplay& displayInfo )
{
	cloneSkinDisplayObjects( env, displayInfo, m_skins );

	m_hardwareSkinning = displayInfo.m_hardwareSkinning;
	m_skinBindings = displayInfo.m_skinBindings;
	m_numSkinBindings = displayInfo.m_numSkinBindings;
	m_skinsLoaded = true;
}

void AnimatedDemoCharacter::initAnimation( const AnimatedDemoCharacterAnimationSet* set )
{
	// Initialize the state machine.
	SimpleBipedStateManager* stateManager = new SimpleBipedStateManager();
	{
		SimpleBipedState* state = new SimpleBipedStandState();
		stateManager->registerState (STAND_STATE, state);
		state->removeReference();

		state = new SimpleBipedWalkState(set->m_walkRunSyncOffset);
		stateManager->registerState (WALK_STATE, state);
		state->removeReference();

		state = new SimpleBipedJumpState();
		stateManager->registerState (JUMP_STATE, state);
		state->removeReference();

		state = new SimpleBipedInAirState();
		stateManager->registerState (IN_AIR_STATE, state);
		state->removeReference();

		state = new SimpleBipedLandState();
		stateManager->registerState (LAND_STATE, state);
		state->removeReference();

		state = new SimpleBipedDiveState();
		stateManager->registerState (DIVE_STATE, state);
		state->removeReference();
	}

	m_animatedSkeleton = new hkaAnimatedSkeleton( set->m_skeleton );

	// Add animations in order
	{
		// MOVE_CONTROL - A Dummy control
		hkaDefaultAnimationControl* control;

		// WALK_CONTROL
		control = new hkaDefaultAnimationControl( HK_NULL );
		control->easeOut(0.0f);
		//control->setMasterWeight(0.01f);
		control->setMasterWeight(HK_REAL_EPSILON);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// IDLE_CONTROL
		control = new hkaDefaultAnimationControl( set->m_idle ); 
		control->easeOut(0.0f);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// JUMP_CONTROL
		control = new hkaDefaultAnimationControl( set->m_jump ); 
		control->easeOut(0.0f);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// IN_AIR_CONTROL
		control = new hkaDefaultAnimationControl( set->m_inAir ); 
		control->easeOut(0.0f);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// LAND_CONTROL
		control = new hkaDefaultAnimationControl( set->m_land ); 
		control->easeOut(0.0f);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// SLOWWALK_CONTROL
		control = new hkaDefaultAnimationControl( set->m_idle ); 
		control->setMasterWeight( 0.0f );
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// WALK_CONTROL
		control = new hkaDefaultAnimationControl( set->m_walk ); 
		control->setMasterWeight( 0.0f );
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// RUN_CONTROL
		control = new hkaDefaultAnimationControl( set->m_run ); 
		// To sync with walk
		control->setLocalTime( set->m_walkRunSyncOffset );
		control->setMasterWeight( 0.0f );
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

		// DIVE_CONTROL
		control = new hkaDefaultAnimationControl( set->m_dive ); 
		control->easeOut(0.0f);
		m_animatedSkeleton->addAnimationControl( control );
		control->removeReference();

	}

	// Initialize the animation command processor
	{
		m_animationMachine = new AnimationEventQueue(m_animatedSkeleton);
		m_animatedSkeleton->removeReference();
	}

	// Initialize the state machine
	{
		m_animationStateMachine = new SimpleBipedStateContext( stateManager );
		stateManager->removeReference();
		m_animationStateMachine->setCurrentState(STAND_STATE, m_animationMachine );
	}

	// Work out motion constraints
	hkaDefaultAnimationControl* walkControl = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( WALK_CONTROL );
	hkaAnimation* walkAnimation = walkControl->getAnimationBinding()->m_animation;
	hkQsTransform walkMotion;
	walkAnimation->getExtractedMotionReferenceFrame(walkAnimation->m_duration, walkMotion );
	m_walkVelocity = hkReal(walkMotion.m_translation.length3()) / walkAnimation->m_duration;

	hkaDefaultAnimationControl* runControl = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( RUN_CONTROL );
	hkaAnimation* runAnimation = runControl->getAnimationBinding()->m_animation;
	hkQsTransform runMotion;
	runAnimation->getExtractedMotionReferenceFrame(runAnimation->m_duration, runMotion );
	m_runVelocity = hkReal(runMotion.m_translation.length3()) / runAnimation->m_duration;
}

void AnimatedDemoCharacter::updatePosition( hkReal timestep, const CharacterStepInput& input, bool& isSupportedOut )
{
	
	{
		hkaDefaultAnimationControl* slowWalkControl = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( SLOW_WALK_CONTROL );
		hkaDefaultAnimationControl* walkControl = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( WALK_CONTROL );
		hkaDefaultAnimationControl* runControl = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( RUN_CONTROL );

		hkReal slowWalkWeight;
		hkReal walkWeight;
		hkReal runWeight;

		if (input.m_forwardVelocity < m_walkVelocity)
		{
			hkReal dampedForwardSpeed = input.m_forwardVelocity/m_walkVelocity;
			walkWeight = dampedForwardSpeed;
			slowWalkWeight = 1.0f - dampedForwardSpeed;
			runWeight = 0.0f;
			walkControl->setPlaybackSpeed( 1.0f );
		}
		else
		{
			hkReal walkSpeed, runSpeed;
			CharacterUtils::computeBlendParams( input.m_forwardVelocity, m_walkVelocity, m_runVelocity, 
				walkControl->getAnimationBinding()->m_animation->m_duration, 
				runControl->getAnimationBinding()->m_animation->m_duration,
				runWeight,
				walkSpeed,
				runSpeed);

			slowWalkWeight = 0.0f;
			walkWeight =  1.0f - runWeight;
			runControl->setPlaybackSpeed( runSpeed );
			walkControl->setPlaybackSpeed( walkSpeed );
		}

		//const hkaDefaultAnimationControl* control = (hkaDefaultAnimationControl*)m_animatedSkeleton->getAnimationControl( MOVE_CONTROL );
		//const hkReal controlWeight = control->getWeight() / control->getMasterWeight();
		const hkReal controlWeight = 1.0f;
		slowWalkControl->setMasterWeight( slowWalkWeight * controlWeight );
		walkControl->setMasterWeight( walkWeight * controlWeight );
 		runControl->setMasterWeight( runWeight * controlWeight );	
	}
	
	// Advance the active animations
	m_animatedSkeleton->stepDeltaTime( timestep );

	const hkUint32 currentAnimationState = m_animationStateMachine->getCurrentState();

	// Check support and filter
	bool supported = m_characterProxy->isSupported( timestep );
	{
		m_timeUnsupported = supported ? 0.0f : m_timeUnsupported + timestep;
		isSupportedOut = supported || (m_timeUnsupported < 0.45f);
	}

	// Compute the motion for the proxy
	hkVector4 extractedMotionCS;
	{
		hkQsTransform desiredMotionAS;
		m_animatedSkeleton->getDeltaReferenceFrame( timestep, desiredMotionAS );
		extractedMotionCS.setRotatedDir( m_characterFromAnimation, desiredMotionAS.getTranslation() );
	}

	hkReal turnAngle = input.m_turnVelocity * timestep;
	{
		hkQuaternion newRotation; newRotation.setAxisAngle( m_characterProxy->getUpLocal(), turnAngle );

		hkTransform wFc;
		m_characterProxy->getTransform( wFc );
		hkRotation r; r.set( newRotation );
		wFc.getRotation().mul( r );
		m_characterProxy->setTransform( wFc );
	}

	// Calculate the velocity we need stateInput order to achieve the desired motion
	hkVector4 desiredVelocityWS;
	{

		hkTransform wFc;
		m_characterProxy->getTransform( wFc );

		hkVector4 desiredMotionWorld;
		desiredMotionWorld.setRotatedDir( wFc.getRotation(), extractedMotionCS );

		// Divide motion by time
		desiredVelocityWS.setMul4 ( 1.0f / timestep, desiredMotionWorld );
	}

	// Adjust velocity
	{
		hkVector4 characterLinearVelocity;
		m_characterProxy->getLinearVelocity( characterLinearVelocity );

		if (currentAnimationState != JUMP_STATE)
		{
			// in these states we ignore the motion coming from the animation
			if (currentAnimationState == IN_AIR_STATE)
			{
				desiredVelocityWS = characterLinearVelocity;
			}
			else
			{
				
				// this is the common case: add the motion velocity to the downward part of the proxy velocity (not upward though)
				hkReal vertComponent = hkMath::min2(0.0f, static_cast<hkReal>(characterLinearVelocity.dot3( m_characterProxy->getUpLocal() )));
				desiredVelocityWS.addMul4(vertComponent, m_characterProxy->getUpLocal() );
			}
		}
		else
		{
			// when jumping, add the motion velocity to the vertical proxy velocity
			hkReal vertComponent = characterLinearVelocity.dot3(m_characterProxy->getUpLocal());
			desiredVelocityWS.addMul4(vertComponent, m_characterProxy->getUpLocal());
		}


		// Just started jumping? Add some extra impulse
		if (m_characterProxy->isSupported(timestep))
		{
			desiredVelocityWS.addMul4( input.m_jumpVelocity, m_characterProxy->getUpLocal() );
		}
		else
		{
			// Add some gravity if not supported
			desiredVelocityWS.addMul4( timestep, m_gravity );
		}
	}

	// Apply velocity to character
	m_characterProxy->setLinearVelocity( desiredVelocityWS );

	// Store turn velocity
	m_characterProxy->setTurnVelocity( input.m_turnVelocity );

}



void AnimatedDemoCharacter::updateDisplay( int numBones, const hkQsTransform* poseMS, hkDemoEnvironment* env )
{
	hkTransform worldFromModel;
	m_characterProxy->getTransform( worldFromModel );
	worldFromModel.getRotation().mul( m_characterFromAnimation ); 

	if ( m_skinsLoaded )
	{
		doSkinning ( numBones, poseMS, worldFromModel, env);
	}
	else
	{
		hkQsTransform tWorldFromModel; tWorldFromModel.setFromTransformNoScale( worldFromModel );
		AnimationUtils::drawPoseModelSpace( *m_animatedSkeleton->getSkeleton(), poseMS, tWorldFromModel );
	}
}

void AnimatedDemoCharacter::display( hkReal timestep, hkDemoEnvironment* env )
{
	HK_TIMER_BEGIN( "AnimatedDemoCharacter::display", HK_NULL );

	hkaPose pose( getSkeleton() );

	updateAnimation( timestep, pose.accessUnsyncedPoseLocalSpace().begin());
	updateDisplay( getSkeleton()->m_numBones, pose.getSyncedPoseModelSpace().begin(),  env);

	HK_TIMER_END();
}


void AnimatedDemoCharacter::updateAnimation( hkReal timestep, hkQsTransform* poseLS )
{
	m_animatedSkeleton->sampleAndCombineAnimations( poseLS, HK_NULL );
}


void AnimatedDemoCharacter::updateMt( hkReal timestep, hkpWorld* world, const CharacterStepInput& _input, struct CharacterActionInfo* actionInfo )
{
	HK_TIMER_BEGIN( "AnimatedDemoCharacter::updateMt", HK_NULL );

	// copy the const input so we can tweak the forward velocity
	CharacterStepInput input = _input;
	hkReal oldForwardVelocity = input.m_forwardVelocity;
	input.m_forwardVelocity *= FORWARD_VELOCITY_SCALAR;

	bool isSupported;
	updatePosition( timestep, input, isSupported );

	SimpleBipedStateInput stateInput;
	stateInput.m_shouldWalk = input.m_forwardVelocity > 0.01f;
	stateInput.m_isSupported = isSupported;
	stateInput.m_shouldJump = input.m_jumpVelocity > 0;
	stateInput.m_shouldDive = actionInfo != HK_NULL ? actionInfo->m_wasDivePressed : (hkBool) false;
	stateInput.m_animMachine = m_animationMachine;
	stateInput.m_context = m_animationStateMachine;

	// Update animation state machine
	m_animationStateMachine->update( timestep, &stateInput );
	m_animationMachine->update( timestep );

	// restore the old forward velocity in case it is being used outside
	input.m_forwardVelocity = oldForwardVelocity;

	HK_TIMER_END();
}


// doSkinning() just renders (skins) the given pose
void AnimatedDemoCharacter::doSkinning (const int boneCount, const hkQsTransform* poseMS, const hkTransform& worldFromModel, hkDemoEnvironment* env )
{
	// Construct the composite world transform
	hkLocalArray<hkTransform> compositeWorldInverse( boneCount );
	compositeWorldInverse.setSize( boneCount );

	// Skin the meshes
	for (int i=0; i < m_numSkinBindings; i++)
	{
		const hkxMesh* inputMesh = m_skinBindings[i]->m_mesh;			
	
		// assumes either a straight map (null map) or a single one (1 palette)
		hkInt16* usedBones = m_skinBindings[i]->m_mappings? m_skinBindings[i]->m_mappings[0].m_mapping : HK_NULL;
		int numUsedBones = usedBones? m_skinBindings[i]->m_mappings[0].m_numMapping : boneCount;

		// Multiply through by the bind pose inverse world inverse matrices
		for (int p=0; p < numUsedBones; p++)
		{
			int boneIndex = usedBones? usedBones[p] : p;
			hkTransform tWorld; poseMS[ boneIndex ].copyToTransform(tWorld);
			compositeWorldInverse[p].setMul( tWorld, m_skinBindings[i]->m_boneFromSkinMeshTransforms[ boneIndex ] );
		}

		// use FPU skinning
		hkgDisplayObject* dispObjForSkin = m_skins[i];
		if (!m_hardwareSkinning)
		{
			// FPU or SIMD skining
			AnimationUtils::skinMesh( *inputMesh, dispObjForSkin, worldFromModel, compositeWorldInverse.begin(), *env->m_sceneConverter );
		}
		else // shader/VU/blend based skinning
		{
			int dindex = hkgAssetConverter::findFirstMapping( env->m_sceneConverter->m_addedSkins, dispObjForSkin );
			hkArray< hkgBlendMatrixSet* >& palettes = env->m_sceneConverter->m_addedSkins[dindex]->m_palettes;
			if (palettes.getSize() < 1)
			{
				HK_WARN( 0x0, "Trying to update a skin with no matrix palettes.");
			}
			hkgAssetConverter::updateSkin( dispObjForSkin, compositeWorldInverse, palettes, worldFromModel, HK_NULL );
		}
	}
}

hkReal AnimatedDemoCharacter::getMaxVelocity() const
{
	return m_runVelocity;
}


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
