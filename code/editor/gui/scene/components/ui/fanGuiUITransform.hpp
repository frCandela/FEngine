#pragma once

#include "scene/components/ui/fanUITransform.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UITransform::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::UiTransform16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UITransform::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui transform";
	}

    //========================================================================================================
    //========================================================================================================
    void UITransform::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UITransform& transform = static_cast<UITransform&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            if( ImGui::Button( "##TransUIPos" ) ){	transform.mPosition = glm::vec2( 0, 0 ); }
            ImGui::SameLine();
            ImGui::DragInt2( "position", &transform.mPosition.x, 1, 0 );

            if( ImGui::Button( "##resetUIsize" ) ) { transform.mSize = glm::ivec2( 100, 100 ); }
            ImGui::SameLine();
            ImGui::DragInt2( "size##sizeUI", &transform.mSize.x, 0.1f );
        }
        ImGui::PopItemWidth();
    }
}