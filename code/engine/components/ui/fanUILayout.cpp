#include "engine/components/ui/fanUILayout.hpp"
#include "engine/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UILayout::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &UILayout::Load;
		_info.save        = &UILayout::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        layout.mType = Type::Vertical;
        layout.mGap = 10;
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Save( const EcsComponent& _component, Json& _json )
	{
        const UILayout& layout = static_cast<const UILayout&>( _component );
        Serializable::SaveInt(_json, "type", (int)layout.mType );
        Serializable::SaveInt(_json, "gap", layout.mGap );
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Load( EcsComponent& _component, const Json& _json )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        Serializable::LoadInt(_json, "type", (int&)layout.mType );
        Serializable::LoadInt(_json, "gap", layout.mGap );
	}
}