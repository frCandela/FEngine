#include "scene/components/ui/fanProgressBar.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ProgressBar::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_PROGRESS_BAR16;
		_info.group = EngineGroups::SceneUI;
		_info.onGui = &ProgressBar::OnGui;
		_info.load = &ProgressBar::Load;
		_info.save = &ProgressBar::Save;
		_info.editorPath = "ui/";
		_info.name = "progress bar";
	}

	//========================================================================================================
	//========================================================================================================
	void ProgressBar::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );
		progressBar.mProgress = 1.f;
		progressBar.mMaxSize  = 100;
		progressBar.mTargetTransform.Init( _world );
	}

	//========================================================================================================
	//========================================================================================================
	void ProgressBar::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			bool update = false;
			update |= ImGui::DragFloat( "progress", &progressBar.mProgress, 0.01f, 0.f, 1.f );
			update |= ImGui::DragInt( "max width", &progressBar.mMaxSize, 0.01f );
			update |= ImGui::FanComponent( "target transform", progressBar.mTargetTransform );

			if( update ) { progressBar. SetProgress( progressBar.mProgress ); }
		} ImGui::PopItemWidth();
	}

	//========================================================================================================
	//========================================================================================================
	void ProgressBar::Save( const EcsComponent& _component, Json& _json )
	{
		const ProgressBar& progressBar = static_cast<const ProgressBar&>( _component );
		Serializable::SaveComponentPtr( _json, "target_ui_transform", progressBar.mTargetTransform );
		Serializable::SaveInt( _json, "max_size", progressBar.mMaxSize );
	}

	//========================================================================================================
	//========================================================================================================
	void ProgressBar::Load( EcsComponent& _component, const Json& _json )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );
		Serializable::LoadComponentPtr( _json, "target_ui_transform", progressBar.mTargetTransform );
		Serializable::LoadInt( _json, "max_size", progressBar.mMaxSize );
	}

	//========================================================================================================
	//========================================================================================================
	void  ProgressBar::SetProgress( const float _progress )
	{
        mProgress = _progress;
		if ( mTargetTransform != nullptr )
		{
            mTargetTransform->mSize.x = int( mProgress * mMaxSize );
		}
	}
}