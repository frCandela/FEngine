#pragma once

#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct TestComponent : public EcsComponent
	{
		ECS_COMPONENT( TestComponent )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		int mValue;
	};
}