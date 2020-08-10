#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	struct UITransform : public EcsComponent
	{
		ECS_COMPONENT( UITransform )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		glm::ivec2 mPosition;
		glm::ivec2 mSize;
	};
}