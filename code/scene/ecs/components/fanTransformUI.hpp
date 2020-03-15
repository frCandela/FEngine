#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UITransform2 : public ecComponent
	{
		DECLARE_COMPONENT( UITransform2 )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _component );
		static void Save( const ecComponent& _component, Json& _json );
		static void Load( ecComponent& _component, const Json& _json );

		glm::vec2 position;
		glm::vec2 scale;
	};
	static constexpr size_t sizeof_transformUI = sizeof( UITransform2 );
}