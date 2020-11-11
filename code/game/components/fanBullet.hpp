#pragma once

#include "core/ecs/fanEcsComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct Bullet : public EcsComponent
	{
		ECS_COMPONENT( Bullet )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		PrefabPtr mExplosionPrefab;
	};
}