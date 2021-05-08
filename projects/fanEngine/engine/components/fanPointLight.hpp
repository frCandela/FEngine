#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
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
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		enum Attenuation { Constant = 0, Linear = 1, Quadratic = 2 };

		Color mAmbiant        = Color::sWhite;
		Color mDiffuse        = Color::sWhite;
		Color mSpecular       = Color::sWhite;
		float mAttenuation[3] = { 0.f, 0.f, 0.1f };

		static float GetLightRange( const PointLight& _light );
	};
}