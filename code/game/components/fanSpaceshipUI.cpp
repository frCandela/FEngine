#include "game/components/fanSpaceShipUI.hpp"

namespace fan
{	
	REGISTER_COMPONENT( SpaceshipUI, "spaceship ui" );

	//================================================================================================================================
	//================================================================================================================================
	void SpaceshipUI::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::SPACE_SHIP16;
		_info.onGui = &SpaceshipUI::OnGui;
		_info.init = &SpaceshipUI::Init;
		_info.load = &SpaceshipUI::Load;
		_info.save = &SpaceshipUI::Save;
		_info.editorPath = "game/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceshipUI::Init( EcsWorld& _world, Component& _component )
	{
		SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );
		spaceshipUI.uiRootTransform.Init( _world );
		spaceshipUI.healthProgress.Init( _world );
		spaceshipUI.energyProgress.Init( _world );
		spaceshipUI.signalProgress.Init( _world );
		spaceshipUI.signalRenderer.Init( _world );
		spaceshipUI.uiOffset = {0, 0};
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceshipUI::OnGui( EcsWorld& _world, EntityID _entityID, Component& _component )
	{
		SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::DragFloat2( "ui offset", &spaceshipUI.uiOffset[0], 1.f );
			ImGui::FanComponent( "ui root transform", spaceshipUI.uiRootTransform );
			ImGui::FanComponent( "health progress", spaceshipUI.healthProgress );
			ImGui::FanComponent( "energy progress", spaceshipUI.energyProgress );
			ImGui::FanComponent( "signal progress", spaceshipUI.signalProgress );
			ImGui::FanComponent( "signal renderer", spaceshipUI.signalRenderer );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceshipUI::Save( const Component& _component, Json& _json )
	{
		const SpaceshipUI& spaceshipUI = static_cast<const SpaceshipUI&>( _component );
		Serializable::SaveVec2(         _json, "ui offset",       spaceshipUI.uiOffset);
		Serializable::SaveComponentPtr( _json, "root_transform",  spaceshipUI.uiRootTransform );
 		Serializable::SaveComponentPtr( _json, "health_progress", spaceshipUI.healthProgress );
 		Serializable::SaveComponentPtr( _json, "energy_progress", spaceshipUI.energyProgress );
 		Serializable::SaveComponentPtr( _json, "signal_progress", spaceshipUI.signalProgress );
 		Serializable::SaveComponentPtr( _json, "signal_renderer", spaceshipUI.signalRenderer );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SpaceshipUI::Load( Component& _component, const Json& _json )
	{
		SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );
		Serializable::LoadVec2(         _json, "ui offset",       spaceshipUI.uiOffset );
		Serializable::LoadComponentPtr( _json, "root_transform",  spaceshipUI.uiRootTransform );
		Serializable::LoadComponentPtr( _json, "health_progress", spaceshipUI.healthProgress );
		Serializable::LoadComponentPtr( _json, "energy_progress", spaceshipUI.energyProgress );
		Serializable::LoadComponentPtr( _json, "signal_progress", spaceshipUI.signalProgress );
		Serializable::LoadComponentPtr( _json, "signal_renderer", spaceshipUI.signalRenderer );
	}
}