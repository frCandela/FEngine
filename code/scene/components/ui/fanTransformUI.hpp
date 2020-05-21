#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "fanGLM.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct TransformUI : public EcsComponent
	{
		ECS_COMPONENT( TransformUI )
	public:
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		glm::vec2 position;
		glm::vec2 scale;
	};
	static constexpr size_t sizeof_transformUI = sizeof( TransformUI );
}