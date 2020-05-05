#pragma  once

#include "ecs/fanComponent.hpp"
#include "fanGLM.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct TransformUI : public Component
	{
		DECLARE_COMPONENT( TransformUI )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		glm::vec2 position;
		glm::vec2 scale;
	};
	static constexpr size_t sizeof_transformUI = sizeof( TransformUI );
}