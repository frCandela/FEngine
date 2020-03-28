#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	// makes a transform follow another transform
	//==============================================================================================================================================================
	struct FollowTransform : public Component
	{
		DECLARE_COMPONENT( FollowTransform )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _material );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );
	};
	static constexpr size_t sizeof_followTransform = sizeof( FollowTransform );
}