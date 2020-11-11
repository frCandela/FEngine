#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorCamera::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Camera16;
		_info.mGroup = EngineGroups::Editor;
		_info.onGui  = &EditorCamera::OnGui;
		_info.mName  = "editor camera";
	}

    //========================================================================================================
    //========================================================================================================
    void EditorCamera::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        EditorCamera& editorCamera = static_cast<EditorCamera&>( _component );
        ImGui::DragFloat( "speed", &editorCamera.mSpeed, 1.f, 0.f, 10000.f );
        ImGui::DragFloat( "speed multiplier", &editorCamera.mSpeedMultiplier, 1.f, 0.f, 10000.f );
        ImGui::DragFloat2( "xy sensitivity", &editorCamera.mXYSensitivity[0], 1.f, 0.f, 1.f );
    }
}