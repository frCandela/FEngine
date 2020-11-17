#include "engine/components/fanMeshRenderer.hpp"
#include <sstream>
#include "core/memory/fanSerializable.hpp"
#include "render/fanRenderSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &MeshRenderer::Load;
		_info.save        = &MeshRenderer::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		meshRenderer.mMesh    = nullptr;
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Save( const EcsComponent& _component, Json& _json )
	{
		const MeshRenderer& meshRenderer = static_cast<const MeshRenderer&>( _component );
		Serializable::SaveMeshPtr( _json, "path", meshRenderer.mMesh );
	}

	//========================================================================================================
	//========================================================================================================
	void MeshRenderer::Load( EcsComponent& _component, const Json& _json )
	{
		MeshRenderer& meshRenderer = static_cast<MeshRenderer&>( _component );
		Serializable::LoadMeshPtr( _json, "path", meshRenderer.mMesh );
	}
}