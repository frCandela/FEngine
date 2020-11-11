#prama once

#include "network/singletons/fanTime.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Time::SetInfo( EcsSingletonInfo& _info )
	{ 
		_info.mIcon  = ImGui::Time16;
		_info.mGroup = EngineGroups::Network;
		_info.onGui  = &Time::OnGui;
		_info.mName  = "time";
	}

	//========================================================================================================
	//========================================================================================================
	void Time::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		Time& gameTime = static_cast<Time&>( _component );
		ImGui::Text( "frame index:          %d", gameTime.mFrameIndex );
		ImGui::Text( "frame start:          %d", gameTime.mFrameStart );
		ImGui::Text( "logic delta:          %.3f", gameTime.mLogicDelta );
		ImGui::Text( "time scale increment: %.3f", gameTime.mTimeScaleIncrement );
		ImGui::DragFloat( "timescale", &gameTime.mTimeScaleDelta, 0.1f );
	}
}