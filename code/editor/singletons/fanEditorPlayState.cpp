#include "editor/singletons/fanEditorPlayState.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorPlayState::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::IconType::PLAY16;
		_info.name = "editor play state";
		_info.group = EngineGroups::Editor;
	}

	//========================================================================================================
	//========================================================================================================
	void EditorPlayState::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
        EditorPlayState& editorPlayState = static_cast<EditorPlayState&>( _component );
        editorPlayState.mState = State::STOPPED;
	}
}