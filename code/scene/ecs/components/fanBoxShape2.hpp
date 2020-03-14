#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEcComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct BoxShape2 : public ecComponent
	{
		DECLARE_COMPONENT( BoxShape2 )
	public:
		BoxShape2();
		static void SetInfo( ComponentInfo& _info );
		static void Init( ecComponent& _component );
		static void OnGui( ecComponent& _boxShape );
		static void Save( const ecComponent& _boxShape, Json& _json );
		static void Load( ecComponent& _boxShape, const Json& _json );
		
		void	  SetScaling( const btVector3 _scaling );
		btVector3 GetScaling() const;

		btBoxShape boxShape;
	};
	static constexpr size_t sizeof_boxShape = sizeof( BoxShape2 );
}