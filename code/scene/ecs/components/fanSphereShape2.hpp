#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SphereShape2 : public ecComponent
	{
		DECLARE_COMPONENT( SphereShape2 )
	public:
		SphereShape2();
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _sphereShape );
		static void Save( const ecComponent& _sphereShape, Json& _json );
		static void Load( ecComponent& _sphereShape, const Json& _json );

		void  SetRadius( const float _radius );
		float GetRadius() const;

		btSphereShape sphereShape;
	};
	static constexpr size_t sizeof_sphereShape = sizeof( SphereShape2 );
}