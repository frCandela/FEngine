#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorSelection::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mName  = "editor selection";
		_info.mIcon  = ImGui::Selection16;
		_info.mGroup = EngineGroups::Editor;
	}
}