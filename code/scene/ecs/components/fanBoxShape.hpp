#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct BoxShape : public Component
	{
		DECLARE_COMPONENT( BoxShape )
	public:
		BoxShape();
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _boxShape );
		static void Save( const Component& _boxShape, Json& _json );
		static void Load( Component& _boxShape, const Json& _json );
		
		void	  SetScaling( const btVector3 _scaling );
		btVector3 GetScaling() const;

		btBoxShape boxShape;
	};
	static constexpr size_t sizeof_boxShape = sizeof( BoxShape );
}