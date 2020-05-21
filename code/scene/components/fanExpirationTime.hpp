#pragma  once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// make an entity die after some time
	//==============================================================================================================================================================
	struct ExpirationTime : public EcsComponent
	{
		ECS_COMPONENT( ExpirationTime )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		float duration;
	};
	static constexpr size_t sizeof_expirationTime = sizeof( ExpirationTime );
}