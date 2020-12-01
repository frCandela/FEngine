#pragma once

#include "editor/singletons/fanEditorGuiInfo.hpp"

#include "project_spaceships/editor/singletons/ui/fanGuiUIMainMenu.hpp"
#include "project_spaceships/editor/singletons/fanGuiClientNetworkManager.hpp"
#include "project_spaceships/editor/singletons/fanGuiCollisionManager.hpp"
#include "project_spaceships/editor/singletons/fanGuiGame.hpp"
#include "project_spaceships/editor/singletons/fanGuiGameCamera.hpp"
#include "project_spaceships/editor/singletons/fanGuiServerNetworkManager.hpp"
#include "project_spaceships/editor/singletons/fanGuiSolarEruption.hpp"
#include "project_spaceships/editor/singletons/fanGuiSunLight.hpp"

#include "project_spaceships/editor/components/fanGuiBattery.hpp"
#include "project_spaceships/editor/components/fanGuiBullet.hpp"
#include "project_spaceships/editor/components/fanGuiDamage.hpp"
#include "project_spaceships/editor/components/fanGuiHealth.hpp"
#include "project_spaceships/editor/components/fanGuiPlanet.hpp"
#include "project_spaceships/editor/components/fanGuiPlayerController.hpp"
#include "project_spaceships/editor/components/fanGuiPlayerInput.hpp"
#include "project_spaceships/editor/components/fanGuiSolarPanel.hpp"
#include "project_spaceships/editor/components/fanGuiSpaceShip.hpp"
#include "project_spaceships/editor/components/fanGuiSpaceshipUI.hpp"
#include "project_spaceships/editor/components/fanGuiWeapon.hpp"

namespace fan
{
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
