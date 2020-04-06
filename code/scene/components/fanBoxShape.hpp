#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// physics box collision shape
	//==============================================================================================================================================================
	struct BoxShape : public Component
	{
		DECLARE_COMPONENT( BoxShape )
	public:
		BoxShape();
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );
		
		void	  SetScaling( const btVector3 _scaling );
		btVector3 GetScaling() const;

		btBoxShape boxShape;
	};
	static constexpr size_t sizeof_boxShape = sizeof( BoxShape );
}