#include "engine/components/ui/fanUITransform.hpp"

#include "core/memory/fanSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UITransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &UITransform::Load;
		_info.save        = &UITransform::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		UITransform& transform = static_cast<UITransform&>( _component );

		transform.mSize     = glm::ivec2( 100, 100);
		transform.mPosition = glm::ivec2( 0, 0 );
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Save( const EcsComponent& _component, Json& _json )
	{
		const UITransform& transform = static_cast<const UITransform&>( _component );

		Serializable::SaveIVec2( _json, "position", transform.mPosition );
		Serializable::SaveIVec2( _json, "size", transform.mSize );
	}

	//========================================================================================================
	//========================================================================================================
	void UITransform::Load( EcsComponent& _component, const Json& _json )
	{
		UITransform& transform = static_cast<UITransform&>( _component );
		Serializable::LoadIVec2( _json, "position", transform.mPosition );
		Serializable::LoadIVec2( _json,"size", transform.mSize );
	}
}