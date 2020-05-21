#include "scene/components/ui/fanProgressBar.hpp"

#include "scene/fanSceneSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::SetInfo( EcsComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::UI_PROGRESS_BAR16;
		_info.onGui = &ProgressBar::OnGui;
		_info.init = &ProgressBar::Init;
		_info.load = &ProgressBar::Load;
		_info.save = &ProgressBar::Save;
		_info.editorPath = "ui/";
		_info.name = "progress bar";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::Init( EcsWorld& _world, EcsComponent& _component )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );
		progressBar.progress = 1.f;
		progressBar.maxScale = 1.f;
		progressBar.targetUiTransform.Init( _world );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			bool update = false;
			update |= ImGui::DragFloat( "progress", &progressBar.progress, 0.01f, 0.f, 1.f );
			update |= ImGui::DragFloat( "max width", &progressBar.maxScale, 0.01f );
			update |= ImGui::FanComponent( "target transform", progressBar.targetUiTransform );

			if( update ) { progressBar. SetProgress( progressBar.progress ); }
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::Save( const EcsComponent& _component, Json& _json )
	{
		const ProgressBar& progressBar = static_cast<const ProgressBar&>( _component );
		Serializable::SaveComponentPtr( _json, "target_ui_transform", progressBar.targetUiTransform );
		Serializable::SaveFloat( _json, "max_scale", progressBar.maxScale );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::Load( EcsComponent& _component, const Json& _json )
	{
		ProgressBar& progressBar = static_cast<ProgressBar&>( _component );
		Serializable::LoadComponentPtr( _json, "target_ui_transform", progressBar.targetUiTransform );
		Serializable::LoadFloat( _json, "max_scale", progressBar.maxScale );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  ProgressBar::SetProgress( const float _progress )
	{
		progress = _progress;
		if ( targetUiTransform != nullptr )
		{
			targetUiTransform->scale.x = progress * maxScale;
		}
	}
}