#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct DirectionalLight2 : public ecComponent
	{
		DECLARE_COMPONENT( DirectionalLight2 )
	public:

		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _light );
		static void Save( const ecComponent& _transform, Json& _json );
		static void Load( ecComponent& _transform, const Json& _json );

		Color ambiant = Color::Black;
		Color diffuse = Color::White;
		Color specular = Color::White;
	};
	static constexpr size_t sizeof_directionalLight = sizeof( DirectionalLight2 );
}