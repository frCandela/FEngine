#pragma once

#include <set>
#include "ecs/fanEcsSingleton.hpp"
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
	struct ScenePointers : public EcsSingleton
	{
		ECS_SINGLETON( ScenePointers )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		std::set< ComponentPtrBase* > unresolvedComponentPtr;

		static void ResolveComponentPointers( Scene& _scene, const uint32_t _idOffset );
	};
}