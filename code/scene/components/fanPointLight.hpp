#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct PointLight : public Component
	{
		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		DECLARE_COMPONENT( PointLight )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _pointLight );
		static void Save( const Component& _pointLight, Json& _json );
		static void Load( Component& _pointLight, const Json& _json );

		Color ambiant = Color::White;
		Color diffuse = Color::White;
		Color specular = Color::White;
		float attenuation[3] = { 0.f,0.f,0.1f };

		static float GetLightRange( const PointLight& _light );
	};
	static constexpr size_t sizeof_pointLight2 = sizeof( PointLight );
}