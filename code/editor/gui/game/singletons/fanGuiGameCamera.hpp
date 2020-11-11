#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void GameCamera::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Camera16;
		_info.mGroup = EngineGroups::Game;
		_info.onGui  = &GameCamera::OnGui;
		_info.mName  = "game camera";
	}

    //========================================================================================================
    //========================================================================================================
    void GameCamera::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        GameCamera& gameCamera = static_cast<GameCamera&>( _component );
        ImGui::DragFloat( "height from target", &gameCamera.mHeightFromTarget, 0.25f, 0.5f, 30.f );
        ImGui::DragFloat2( "margin ratio", &gameCamera.mMarginRatio[0], 0.1f, 0.f, 10.f );
        ImGui::DragFloat( "minSize", &gameCamera.mMinOrthoSize, 0.1f, 0.f, 100.f );
    }

}