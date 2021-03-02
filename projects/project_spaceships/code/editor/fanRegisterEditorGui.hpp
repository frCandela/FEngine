#pragma once

#ifdef FAN_EDITOR

#include "editor/singletons/fanEditorGuiInfo.hpp"

#include "editor/singletons/ui/fanGuiUIMainMenu.hpp"
#include "editor/singletons/fanGuiClientNetworkManager.hpp"
#include "editor/singletons/fanGuiCollisionManager.hpp"
#include "editor/singletons/fanGuiGame.hpp"
#include "editor/singletons/fanGuiGameCamera.hpp"
#include "editor/singletons/fanGuiServerNetworkManager.hpp"
#include "editor/singletons/fanGuiSolarEruption.hpp"
#include "editor/singletons/fanGuiSunLight.hpp"

#include "editor/components/fanGuiBattery.hpp"
#include "editor/components/fanGuiBullet.hpp"
#include "editor/components/fanGuiDamage.hpp"
#include "editor/components/fanGuiHealth.hpp"
#include "editor/components/fanGuiPlanet.hpp"
#include "editor/components/fanGuiPlayerController.hpp"
#include "editor/components/fanGuiPlayerInput.hpp"
#include "editor/components/fanGuiSolarPanel.hpp"
#include "editor/components/fanGuiSpaceShip.hpp"
#include "editor/components/fanGuiSpaceshipUI.hpp"
#include "editor/components/fanGuiWeapon.hpp"

namespace fan
{
    //========================================================================================================
    // Registers gui informations when launching with an Editor container
    //========================================================================================================
    inline void RegisterEditorGuiInfos( EditorGuiInfo& _editorGui )
    {
        _editorGui.mSingletonInfos[ UIMainMenu::Info::sType ]            = GuiUIMainMenu::GetInfo();
        _editorGui.mSingletonInfos[ ClientNetworkManager::Info::sType ]  = GuiClientNetworkManager::GetInfo();
        _editorGui.mSingletonInfos[ CollisionManager::Info::sType ]      = GuiCollisionManager::GetInfo();
        _editorGui.mSingletonInfos[ Game::Info::sType ]                  = GuiGame::GetInfo();
        _editorGui.mSingletonInfos[ GameCamera::Info::sType ]            = GuiGameCamera::GetInfo();
        _editorGui.mSingletonInfos[ ServerNetworkManager::Info::sType ]  = GuiServerNetworkManager::GetInfo();
        _editorGui.mSingletonInfos[ SolarEruption::Info::sType ]         = GuiSolarEruption::GetInfo();
        _editorGui.mSingletonInfos[ SunLight::Info::sType ]              = GuiSunLight::GetInfo();

        _editorGui.mComponentInfos[ Battery::Info::sType ]           = GuiBattery::GetInfo();
        _editorGui.mComponentInfos[ Bullet::Info::sType ]            = GuiBullet::GetInfo();
        _editorGui.mComponentInfos[ Damage::Info::sType ]            = GuiDamage::GetInfo();
        _editorGui.mComponentInfos[ Health::Info::sType ]            = GuiHealth::GetInfo();
        _editorGui.mComponentInfos[ Planet::Info::sType ]            = GuiPlanet::GetInfo();
        _editorGui.mComponentInfos[ PlayerController::Info::sType ]  = GuiPlayerController::GetInfo();
        _editorGui.mComponentInfos[ PlayerInput::Info::sType ]       = GuiPlayerInput::GetInfo();
        _editorGui.mComponentInfos[ SolarPanel::Info::sType ]        = GuiSolarPanel::GetInfo();
        _editorGui.mComponentInfos[ SpaceShip::Info::sType ]         = GuiSpaceShip::GetInfo();
        _editorGui.mComponentInfos[ SpaceshipUI::Info::sType ]       = GuiSpaceshipUI::GetInfo();
        _editorGui.mComponentInfos[ Weapon::Info::sType ]            = GuiWeapon::GetInfo();
    }
}

#endif
