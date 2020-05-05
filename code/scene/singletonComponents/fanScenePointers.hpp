#pragma once

#include <set>
#include "ecs/fanSingletonComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	struct Scene;
	class Prefab;

	//================================================================================================================================
	// Contains all scene pointers that need to be resolved
	// unresolved pointers appear when loading scenes or instancing prefabs
	//================================================================================================================================
	struct ScenePointers : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( EcsWorld&, SingletonComponent& _component );

		std::set< ComponentPtrBase* > unresolvedComponentPtr;

		static void ResolveComponentPointers( Scene& _scene, const uint32_t _idOffset );
	};
}