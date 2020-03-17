#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct SphereShape : public Component
	{
		DECLARE_COMPONENT( SphereShape )
	public:
		SphereShape();
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _sphereShape );
		static void Save( const Component& _sphereShape, Json& _json );
		static void Load( Component& _sphereShape, const Json& _json );

		void  SetRadius( const float _radius );
		float GetRadius() const;

		btSphereShape sphereShape;
	};
	static constexpr size_t sizeof_sphereShape = sizeof( SphereShape );
}