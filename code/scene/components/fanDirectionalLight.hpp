#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// 3D dirctionnal light for rendering
	//==============================================================================================================================================================
	struct DirectionalLight : public EcsComponent
	{
		ECS_COMPONENT( DirectionalLight )
	public:

		enum Attenuation { CONSTANT = 0, LINEAR = 1, QUADRATIC = 2 };

		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		Color ambiant;
		Color diffuse;
		Color specular;
	};
	static constexpr size_t sizeof_directionalLight = sizeof( DirectionalLight );
}