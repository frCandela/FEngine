#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct TransformUI : public ecComponent
	{
		DECLARE_COMPONENT( TransformUI )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _component );
		static void Save( const ecComponent& _component, Json& _json );
		static void Load( ecComponent& _component, const Json& _json );

		glm::ivec2 position;
		glm::ivec2 size;
	};
	static constexpr size_t sizeof_transformUI = sizeof( TransformUI );
}