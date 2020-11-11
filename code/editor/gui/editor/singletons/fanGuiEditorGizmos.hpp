#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorGizmos::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mName  = "editor gizmo";
		_info.mIcon  = ImGui::Gizmos16;
		_info.mGroup = EngineGroups::Editor;
	}
}