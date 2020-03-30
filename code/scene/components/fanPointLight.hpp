#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct PointLight : public Component
	{

		DECLARE_COMPONENT( PointLight )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( EcsWorld& _world, EntityID _entityID, Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );

		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		Color ambiant = Color::White;
		Color diffuse = Color::White;
		Color specular = Color::White;
		float attenuation[3] = { 0.f,0.f,0.1f };

		static float GetLightRange( const PointLight& _light );
	};
	static constexpr size_t sizeof_pointLight2 = sizeof( PointLight );
}