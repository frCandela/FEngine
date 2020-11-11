#prama once

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
}