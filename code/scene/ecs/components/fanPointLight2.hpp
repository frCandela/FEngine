#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct PointLight2 : public ecComponent
	{
		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		DECLARE_COMPONENT( PointLight2 )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _pointLight );
		static void Save( const ecComponent& _pointLight, Json& _json );
		static void Load( ecComponent& _pointLight, const Json& _json );

		Color ambiant = Color::White;
		Color diffuse = Color::White;
		Color specular = Color::White;
		float attenuation[3] = { 0.f,0.f,0.1f };

		static float GetLightRange( const PointLight2& _light );
	};
	static constexpr size_t sizeof_pointLight2 = sizeof( PointLight2 );
}