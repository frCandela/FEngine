#pragma  once

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// make an entity die after some time
	//==============================================================================================================================================================
	struct ExpirationTime : public Component
	{
		DECLARE_COMPONENT( ExpirationTime )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		float duration;
	};
	static constexpr size_t sizeof_expirationTime = sizeof( ExpirationTime );
}