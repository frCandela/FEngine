#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorCopyPaste::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mName  = "editor copy/paste";
		_info.mIcon  = ImGui::CopyPaste16;
		_info.mGroup = EngineGroups::Editor;
	}
} 