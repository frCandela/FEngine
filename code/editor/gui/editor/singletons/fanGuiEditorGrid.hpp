#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EditorGrid::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Grid16;
		_info.mName  = "editor grid";
		_info.mGroup = EngineGroups::Editor;
        _info.onGui  = EditorGrid::OnGui;
	}

    //========================================================================================================
    //========================================================================================================
    void EditorGrid::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        EditorGrid& grid = static_cast<EditorGrid&>( _component );
        ImGui::MenuItem( "visible", nullptr, &grid.mIsVisible );
        ImGui::DragFloat( "spacing", &grid.mSpacing, 0.25f, 0.f, 100.f );
        ImGui::DragInt( "lines count", &grid.mLinesCount, 1.f, 0, 1000 );
        ImGui::ColorEdit4( "color", &grid.mColor[0], ImGui::fanColorEditFlags );
        ImGui::DragFloat3( "offset", &grid.mOffset[0] );
    }
}