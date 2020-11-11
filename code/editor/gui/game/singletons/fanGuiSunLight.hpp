#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SunLight::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::Sun16;
		_info.mGroup = EngineGroups::Game;
		_info.onGui  = &SunLight::OnGui;
		_info.mName  = "sun light";
	}

    //========================================================================================================
    //========================================================================================================
    void SunLight::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        SunLight& sunLight = static_cast<SunLight&>( _component );
        ImGui::DragFloat( "sub angle", &sunLight.mSubAngle, 1.f, 0.f, 90.f );
        ImGui::DragFloat( "radius", &sunLight.mRadius, 1.f, 1.f, 1000.f );
    }
}