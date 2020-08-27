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
		_info.load        = &UIProgressBar::Load;
		_info.save        = &UIProgressBar::Save;
		_info.mEditorPath = "ui/";
		_info.mName       = "ui progress bar";
	}

	//========================================================================================================
	//========================================================================================================
	void UIProgressBar::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		UIProgressBar& progressBar = static_cast<UIProgressBar&>( _component );
		progressBar.mProgress = 1.f;
		progressBar.mMaxSize  = 100;
		progressBar.mTargetTransform.Init( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void UIProgressBar::Save( const EcsComponent& _component, Json& _json )
	{
		const UIProgressBar& progressBar = static_cast<const UIProgressBar&>( _component );
		Serializable::SaveComponentPtr( _json, "target_ui_transform", progressBar.mTargetTransform );
		Serializable::SaveInt( _json, "max_size", progressBar.mMaxSize );
	}

	//========================================================================================================
	//========================================================================================================
	void UIProgressBar::Load( EcsComponent& _component, const Json& _json )
	{
		UIProgressBar& progressBar = static_cast<UIProgressBar&>( _component );
		Serializable::LoadComponentPtr( _json, "target_ui_transform", progressBar.mTargetTransform );
		Serializable::LoadInt( _json, "max_size", progressBar.mMaxSize );
	}

	//========================================================================================================
	//========================================================================================================
	void  UIProgressBar::SetProgress( const float _progress )
	{
        mProgress = _progress;
		if ( mTargetTransform != nullptr )
		{
            mTargetTransform->mSize.x = int( mProgress * mMaxSize );
		}
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