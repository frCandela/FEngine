#pragma once

#include "core/ecs/fanEcsComponent.hpp"

namespace fan
{
	//========================================================================================================
	// The health of en entity
	//========================================================================================================
	class Health : public EcsComponent
	{
		ECS_COMPONENT( Health )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool  mInvincible    = false;
		float mCurrentHealth = 0.f;
		float mMaxHealth     = 100.f;
	};
}