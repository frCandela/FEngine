#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct DirectionalLight : public EcsComponent
	{
		ECS_COMPONENT( DirectionalLight )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Color mAmbiant;
		Color mDiffuse;
		Color mSpecular;
	};
}