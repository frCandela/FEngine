#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorPlayState::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::IconType::Play16;
		_info.mName  = "editor play state";
		_info.mGroup = EngineGroups::Editor;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorPlayState::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
        EditorPlayState& editorPlayState = static_cast<EditorPlayState&>( _component );
        editorPlayState.mState = State::STOPPED;
	}
}