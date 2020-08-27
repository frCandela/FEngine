#include "game/components/fanSpaceShipUI.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SpaceshipUI::SetInfo( EcsComponentInfo& _info )
	{
		_info.mIcon       = ImGui::IconType::Spaceship16;
		_info.mGroup      = EngineGroups::Game;
		_info.onGui       = &SpaceshipUI::OnGui;
		_info.load        = &SpaceshipUI::Load;
		_info.save        = &SpaceshipUI::Save;
		_info.mEditorPath = "game/";
		_info.mName       = "spaceship ui";
	}

	//========================================================================================================
	//========================================================================================================
	void SpaceshipUI::Init( EcsWorld& _world, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );
		spaceshipUI.mUIRootTransform.Init( _world );
		spaceshipUI.mHealthProgress.Init( _world );
		spaceshipUI.mEnergyProgress.Init( _world );
		spaceshipUI.mSignalProgress.Init( _world );
		spaceshipUI.mSignalRenderer.Init( _world );
		spaceshipUI.mUIOffset = { 0, 0};
	}

	//========================================================================================================
	//========================================================================================================
	void SpaceshipUI::Save( const EcsComponent& _component, Json& _json )
	{
		const SpaceshipUI& spaceshipUI = static_cast<const SpaceshipUI&>( _component );
		Serializable::SaveVec2(         _json, "ui offset",       spaceshipUI.mUIOffset);
		Serializable::SaveComponentPtr( _json, "root_transform",  spaceshipUI.mUIRootTransform );
 		Serializable::SaveComponentPtr( _json, "health_progress", spaceshipUI.mHealthProgress );
 		Serializable::SaveComponentPtr( _json, "energy_progress", spaceshipUI.mEnergyProgress );
 		Serializable::SaveComponentPtr( _json, "signal_progress", spaceshipUI.mSignalProgress );
 		Serializable::SaveComponentPtr( _json, "signal_renderer", spaceshipUI.mSignalRenderer );
	}

	//========================================================================================================
	//========================================================================================================
	void SpaceshipUI::Load( EcsComponent& _component, const Json& _json )
	{
		SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );
		Serializable::LoadVec2(         _json, "ui offset",       spaceshipUI.mUIOffset );
		Serializable::LoadComponentPtr( _json, "root_transform",  spaceshipUI.mUIRootTransform );
		Serializable::LoadComponentPtr( _json, "health_progress", spaceshipUI.mHealthProgress );
		Serializable::LoadComponentPtr( _json, "energy_progress", spaceshipUI.mEnergyProgress );
		Serializable::LoadComponentPtr( _json, "signal_progress", spaceshipUI.mSignalProgress );
		Serializable::LoadComponentPtr( _json, "signal_renderer", spaceshipUI.mSignalRenderer );
	}

    //========================================================================================================
    //========================================================================================================
    void SpaceshipUI::OnGui( EcsWorld& /*_world*/, EcsEntity /*_entityID*/, EcsComponent& _component )
    {
        SpaceshipUI& spaceshipUI = static_cast<SpaceshipUI&>( _component );

        ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
        {
            ImGui::DragFloat2( "ui offset", &spaceshipUI.mUIOffset[0], 1.f );
            ImGui::FanComponent( "ui root transform", spaceshipUI.mUIRootTransform );
            ImGui::FanComponent( "health progress", spaceshipUI.mHealthProgress );
            ImGui::FanComponent( "energy progress", spaceshipUI.mEnergyProgress );
            ImGui::FanComponent( "signal progress", spaceshipUI.mSignalProgress );
            ImGui::FanComponent( "signal renderer", spaceshipUI.mSignalRenderer );
        } ImGui::PopItemWidth();
    }
}