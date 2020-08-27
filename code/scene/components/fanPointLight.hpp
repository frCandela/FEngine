#pragma  once

#include "ecs/fanEcsComponent.hpp"
#include "core/fanColor.hpp"

namespace fan
{
	//========================================================================================================
	// a point light for 3D illumination 
	//========================================================================================================
	struct PointLight : public EcsComponent
	{
		ECS_COMPONENT( PointLight )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum Attenuation { Constant = 0, Linear = 1, Quadratic = 2 };

		Color mAmbiant        = Color::White;
		Color mDiffuse        = Color::White;
		Color mSpecular       = Color::White;
		float mAttenuation[3] = { 0.f, 0.f, 0.1f };

		static float GetLightRange( const PointLight& _light );
	};
}