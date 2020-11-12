#include "editor/singletons/fanEditorGuiInfo.hpp"

#include "editor/gui/editor/singletons/fanGuiEditorCamera.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorCopyPaste.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorGizmos.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorGrid.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorPlayState.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorSelection.hpp"

#include "editor/gui/network/singletons/fanGuiHostManager.hpp"
#include "editor/gui/network/singletons/fanGuiLinkingContext.hpp"
#include "editor/gui/network/singletons/fanGuiServerConnection.hpp"
#include "editor/gui/network/singletons/fanGuiSpawnManager.hpp"
#include "editor/gui/network/singletons/fanGuiTime.hpp"

#include "editor/gui/scene/singletons/fanGuiApplication.hpp"
#include "editor/gui/scene/singletons/fanGuiMouse.hpp"
#include "editor/gui/scene/singletons/fanGuiPhysicsWorld.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderDebug.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderResources.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderWorld.hpp"
#include "editor/gui/scene/singletons/fanGuiScene.hpp"
#include "editor/gui/scene/singletons/fanGuiScenePointers.hpp"
#include "editor/gui/scene/singletons/fanGuiSceneResources.hpp"

#include "editor/gui/game/singletons/ui/fanGuiUIMainMenu.hpp"
#include "editor/gui/game/singletons/fanGuiClientNetworkManager.hpp"
#include "editor/gui/game/singletons/fanGuiCollisionManager.hpp"
#include "editor/gui/game/singletons/fanGuiGame.hpp"
#include "editor/gui/game/singletons/fanGuiGameCamera.hpp"
#include "editor/gui/game/singletons/fanGuiServerNetworkManager.hpp"
#include "editor/gui/game/singletons/fanGuiSolarEruption.hpp"
#include "editor/gui/game/singletons/fanGuiSunLight.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void EditorGuiInfo::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //========================================================================================================
    //========================================================================================================
    void EditorGuiInfo::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        EditorGuiInfo& editorGui = static_cast<EditorGuiInfo&>( _component );

        //editor singletons
        editorGui.mSingletonInfos[ EditorCamera::Info::sType ]      = GuiEditorCamera::GetInfo();
        editorGui.mSingletonInfos[ EditorCopyPaste::Info::sType ]   = GuiEditorCopyPaste::GetInfo();
        editorGui.mSingletonInfos[ EditorGizmos::Info::sType ]      = GuiEditorGizmos::GetInfo();
        editorGui.mSingletonInfos[ EditorGrid::Info::sType ]        = GuiEditorGrid::GetInfo();
        editorGui.mSingletonInfos[ EditorPlayState::Info::sType ]   = GuiEditorPlayState::GetInfo();
        editorGui.mSingletonInfos[ EditorSelection::Info::sType ]   = GuiEditorSelection::GetInfo();

        //network singletons
        editorGui.mSingletonInfos[ HostManager::Info::sType ]       = GuiHostManager::GetInfo();
        editorGui.mSingletonInfos[ LinkingContext::Info::sType ]    = GuiLinkingContext::GetInfo();
        editorGui.mSingletonInfos[ ServerConnection::Info::sType ]  = GuiServerConnection::GetInfo();
        editorGui.mSingletonInfos[ SpawnManager::Info::sType ]      = GuiSpawnManager::GetInfo();
        editorGui.mSingletonInfos[ Time::Info::sType ]              = GuiTime::GetInfo();

        // scene singletons
        editorGui.mSingletonInfos[ Application::Info::sType ]       = GuiApplication::GetInfo();
        editorGui.mSingletonInfos[ Mouse::Info::sType ]             = GuiMouse::GetInfo();
        editorGui.mSingletonInfos[ PhysicsWorld::Info::sType ]      = GuiPhysicsWorld::GetInfo();
        editorGui.mSingletonInfos[ RenderDebug::Info::sType ]       = GuiRenderDebug::GetInfo();
        editorGui.mSingletonInfos[ RenderResources::Info::sType ]   = GuiRenderResources::GetInfo();
        editorGui.mSingletonInfos[ RenderWorld::Info::sType ]       = GuiRenderWorld::GetInfo();
        editorGui.mSingletonInfos[ Scene::Info::sType ]             = GuiScene::GetInfo();
        editorGui.mSingletonInfos[ ScenePointers::Info::sType ]     = GuiScenePointers::GetInfo();
        editorGui.mSingletonInfos[ SceneResources::Info::sType ]    = GuiSceneResources::GetInfo();

        // game singletons
        editorGui.mSingletonInfos[ UIMainMenu::Info::sType ]            = GuiUIMainMenu::GetInfo();
        editorGui.mSingletonInfos[ ClientNetworkManager::Info::sType ]  = GuiClientNetworkManager::GetInfo();
        editorGui.mSingletonInfos[ CollisionManager::Info::sType ]      = GuiCollisionManager::GetInfo();
        editorGui.mSingletonInfos[ Game::Info::sType ]                  = GuiGame::GetInfo();
        editorGui.mSingletonInfos[ GameCamera::Info::sType ]            = GuiGameCamera::GetInfo();
        editorGui.mSingletonInfos[ ServerNetworkManager::Info::sType ]  = GuiServerNetworkManager::GetInfo();
        editorGui.mSingletonInfos[ SolarEruption::Info::sType ]         = GuiSolarEruption::GetInfo();
        editorGui.mSingletonInfos[ SunLight::Info::sType ]              = GuiSunLight::GetInfo();
    }

    //========================================================================================================
    //========================================================================================================
    const GuiComponentInfo&	EditorGuiInfo::GetComponentInfo( const uint32_t _type ) const
    {
        return mComponentInfos.at( _type );
    }

    //========================================================================================================
    //========================================================================================================
    const GuiSingletonInfo& EditorGuiInfo::GetSingletonInfo( const uint32_t _type ) const
    {
        return mSingletonInfos.at( _type );
    }
}
