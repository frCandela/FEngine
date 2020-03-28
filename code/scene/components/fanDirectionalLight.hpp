#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct DirectionalLight : public Component
	{
		DECLARE_COMPONENT( DirectionalLight )
	public:

		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );
		static void OnGui( Component& _light );
		static void Save( const Component& _transform, Json& _json );
		static void Load( Component& _transform, const Json& _json );

		Color ambiant;
		Color diffuse;
		Color specular;
	};
	static constexpr size_t sizeof_directionalLight = sizeof( DirectionalLight );
}