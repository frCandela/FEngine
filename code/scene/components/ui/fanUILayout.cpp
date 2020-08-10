#include "scene/components/ui/fanUILayout.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UILayout::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::NONE;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &UILayout::OnGui;
		_info.load = &UILayout::Load;
		_info.save = &UILayout::Save;
		_info.editorPath = "ui/";
		_info.name = "ui layout";
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        (void)layout;
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Save( const EcsComponent& _component, Json& _json )
	{
        const UILayout& layout = static_cast<const UILayout&>( _component );
        (void)layout;
        (void)_json;
	}

	//========================================================================================================
	//========================================================================================================
	void UILayout::Load( EcsComponent& _component, const Json& _json )
	{
        UILayout& layout = static_cast<UILayout&>( _component );
        (void)layout;
        (void)_json;
	}

    //========================================================================================================
    //========================================================================================================
    void UILayout::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UILayout& layout = static_cast<UILayout&>( _component );
        (void)layout;

    }
}