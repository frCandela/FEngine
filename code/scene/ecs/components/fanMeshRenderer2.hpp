#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/ecs/fanEntity.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	struct ComponentInfo;
	class Mesh;

	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct MeshRenderer2 : public ecComponent
	{
		DECLARE_COMPONENT( MeshRenderer2 )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Clear( ecComponent& _meshRenderer );
		static void OnGui( ecComponent& _meshRenderer );
		static void Save( const ecComponent& _meshRenderer, Json& _json );
		static void Load( ecComponent& _meshRenderer, const Json& _json );

		MeshPtr mesh;
		int renderID = -1;
	};
	static constexpr size_t sizeof_meshRenderer = sizeof( MeshRenderer2 );
}