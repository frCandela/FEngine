#prama once

#include "scene/components/ui/fanUILayout.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UILayout::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Layout16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UILayout::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui layout";
	}

    //========================================================================================================
    //========================================================================================================
    void UILayout::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        UILayout& layout = static_cast<UILayout&>( _component );
        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() + 16 );
        ImGui::Combo("type", (int*)&layout.mType, "horizontal\0vertical\0\0");
        ImGui::DragInt( "gap", &layout.mGap, 1.f, -1, 100 );
        ImGui::PopItemWidth();
    }
}