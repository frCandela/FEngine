#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;
	struct SceneNode;
	struct Scene;
	class Prefab;
	struct ComponentPtrBase;
    class SlotPtr;

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

		std::vector< SlotPtr* >          mUnresolvedSlotPtr;
        std::vector< ComponentPtrBase* > mUnresolvedComponentPtr;

        static void Clear( ScenePointers& _scenePointers );
		static void ResolveComponentPointers( EcsWorld& _world, const uint32_t _idOffset );
	};
}