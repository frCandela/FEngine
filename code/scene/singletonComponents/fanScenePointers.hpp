#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	struct Scene;
	class Prefab;

	//================================================================================================================================
	// contains all scene pointers that need to be resolved
	// unresolved pointer appear when loading scenes or instancing prefabs
	//================================================================================================================================
	struct ScenePointers : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );

		std::set< ComponentPtrBase* > unresolvedComponentPtr;

		static void ResolveComponentPointers( Scene& _scene, const uint32_t _idOffset );
	};
}