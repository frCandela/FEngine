#pragma  once

#include "ecs/fanEcsComponent.hpp"

namespace fan
{
    struct Mesh2D;

	//========================================================================================================
	//========================================================================================================
	struct UIText : public EcsComponent
	{
		ECS_COMPONENT( UIText )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		std::string mText;
		Mesh2D *    mMesh2D;
	};
}