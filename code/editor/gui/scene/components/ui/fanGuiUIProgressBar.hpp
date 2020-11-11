#prama once

#include "scene/components/ui/fanUIProgressBar.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void UIProgressBar::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::UiProgressBar16;
		_info.mGroup      = EngineGroups::SceneUI;
		_info.onGui       = &UIProgressBar::OnGui;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui progress bar";
	}

    //========================================================================================================
    //========================================================================================================
    void UIProgressBar::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        UIProgressBar& progressBar = static_cast<UIProgressBar&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            bool update = false;
            update |= ImGui::DragFloat( "progress", &progressBar.mProgress, 0.01f, 0.f, 1.f );
            update |= ImGui::DragInt( "max width", &progressBar.mMaxSize, 0.01f );
            update |= ImGui::FanComponent( "target transform", progressBar.mTargetTransform );

            if( update ) { progressBar. SetProgress( progressBar.mProgress ); }
        } ImGui::PopItemWidth();
    }
}