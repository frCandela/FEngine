#include "editor/singletons/fanEditorGuiInfo.hpp"

// EDITOR
#include "editor/singletons/fanEditorCamera.hpp"
#include "editor/singletons/fanEditorCopyPaste.hpp"
#include "editor/singletons/fanEditorGizmos.hpp"
#include "editor/singletons/fanEditorGrid.hpp"
#include "editor/singletons/fanEditorPlayState.hpp"
#include "editor/singletons/fanEditorSelection.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/singletons/fanEditorMainMenuBar.hpp"
#include "editor/windows/fanConsoleWindow.hpp"
#include "editor/windows/fanEcsWindow.hpp"
#include "editor/windows/fanInspectorWindow.hpp"
#include "editor/windows/fanPreferencesWindow.hpp"
#include "editor/windows/fanProfilerWindow.hpp"
#include "editor/windows/fanGameViewWindow.hpp"
#include "editor/windows/fanRenderWindow.hpp"
#include "editor/windows/fanSceneWindow.hpp"
#include "editor/windows/fanSingletonsWindow.hpp"
#include "editor/windows/fanTerrainWindow.hpp"
#include "editor/windows/fanUnitsTestsWindow.hpp"

// NETWORK
#include "editor/gui/network/fanGuiHostManager.hpp"
#include "editor/gui/network/fanGuiLinkingContext.hpp"
#include "editor/gui/network/fanGuiServerConnection.hpp"
#include "editor/gui/network/fanGuiSpawnManager.hpp"
#include "editor/gui/network/fanGuiTime.hpp"

#include "editor/gui/network/fanGuiClientConnection.hpp"
#include "editor/gui/network/fanGuiClientGameData.hpp"
#include "editor/gui/network/fanGuiClientReplication.hpp"
#include "editor/gui/network/fanGuiClientRollback.hpp"
#include "editor/gui/network/fanGuiClientRPC.hpp"
#include "editor/gui/network/fanGuiEntityReplication.hpp"
#include "editor/gui/network/fanGuiHostConnection.hpp"
#include "editor/gui/network/fanGuiHostGameData.hpp"
#include "editor/gui/network/fanGuiHostPersistentHandle.hpp"
#include "editor/gui/network/fanGuiHostReplication.hpp"
#include "editor/gui/network/fanGuiLinkingContextUnregisterer.hpp"
#include "editor/gui/network/fanGuiReliabilityLayer.hpp"

// ENGINE
#include "editor/gui/singletons/fanGuiApplication.hpp"
#include "editor/gui/singletons/fanGuiMouse.hpp"
#include "editor/gui/singletons/fanGuiRenderDebug.hpp"
#include "editor/gui/singletons/fanGuiRenderResources.hpp"
#include "editor/gui/singletons/fanGuiRenderWorld.hpp"
#include "editor/gui/singletons/fanGuiScene.hpp"
#include "editor/gui/singletons/fanGuiScenePointers.hpp"
#include "editor/gui/singletons/fanGuiSceneResources.hpp"
#include "editor/gui/singletons/fanGuiVoxelTerrain.hpp"

#include "editor/gui/ui/fanGuiUIAlign.hpp"
#include "editor/gui/ui/fanGuiUIButton.hpp"
#include "editor/gui/ui/fanGuiUILayout.hpp"
#include "editor/gui/ui/fanGuiUIProgressBar.hpp"
#include "editor/gui/ui/fanGuiUIRenderer.hpp"
#include "editor/gui/ui/fanGuiUIText.hpp"
#include "editor/gui/ui/fanGuiUITransform.hpp"

#include "editor/gui/components/fanGuiBounds.hpp"
#include "editor/gui/components/fanGuiCamera.hpp"
#include "editor/gui/components/fanGuiDirectionalLight.hpp"
#include "editor/gui/components/fanGuiExpirationTime.hpp"
#include "editor/gui/components/fanGuiFollowTransform.hpp"
#include "editor/gui/components/fanGuiMaterial.hpp"
#include "editor/gui/components/fanGuiMeshRenderer.hpp"
#include "editor/gui/components/fanGuiParticle.hpp"
#include "editor/gui/components/fanGuiParticleEmitter.hpp"
#include "editor/gui/components/fanGuiPointLight.hpp"
#include "editor/gui/components/fanGuiSceneNode.hpp"
#include "editor/gui/components/fanGuiFxScale.hpp"

#include "editor/gui/physics/fanGuiFxTransform.hpp"
#include "editor/gui/physics/fanGuiFxRigidbody.hpp"
#include "editor/gui/physics/fanGuiFxSphereCollider.hpp"
#include "editor/gui/physics/fanGuiFxBoxCollider.hpp"
#include "editor/gui/physics/fanGuiFxPhysicsWorld.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGuiInfo::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGuiInfo::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        EditorGuiInfo& editorGui = static_cast<EditorGuiInfo&>( _singleton );
        editorGui.mSingletonInfos.clear();
        editorGui.mComponentInfos.clear();

        //editor
        editorGui.mSingletonInfos[EditorGuiInfo::Info::sType]     = GuiEditorGuiInfo::GetInfo();
        editorGui.mSingletonInfos[EditorCamera::Info::sType]      = GuiEditorCamera::GetInfo();
        editorGui.mSingletonInfos[EditorCopyPaste::Info::sType]   = GuiEditorCopyPaste::GetInfo();
        editorGui.mSingletonInfos[EditorGizmos::Info::sType]      = GuiEditorGizmos::GetInfo();
        editorGui.mSingletonInfos[EditorGrid::Info::sType]        = GuiEditorGrid::GetInfo();
        editorGui.mSingletonInfos[EditorPlayState::Info::sType]   = GuiEditorPlayState::GetInfo();
        editorGui.mSingletonInfos[EditorSelection::Info::sType]   = GuiEditorSelection::GetInfo();
        editorGui.mSingletonInfos[EditorSettings::Info::sType]    = GuiEditorSettings::GetInfo();
        editorGui.mSingletonInfos[EditorMainMenuBar::Info::sType] = GuiEditorMainMenuBar::GetInfo();


        // editor windows
        editorGui.mSingletonInfos[ConsoleWindow::Info::sType]     = GuiConsoleWindow::GetInfo();
        editorGui.mSingletonInfos[EcsWindow::Info::sType]         = GuiEcsWindow::GetInfo();
        editorGui.mSingletonInfos[InspectorWindow::Info::sType]   = GuiInspectorWindow::GetInfo();
        editorGui.mSingletonInfos[PreferencesWindow::Info::sType] = GuiPreferencesWindow::GetInfo();
        editorGui.mSingletonInfos[ProfilerWindow::Info::sType]    = GuiProfilerWindow::GetInfo();
        editorGui.mSingletonInfos[GameViewWindow::Info::sType]    = GuiGameViewWindow::GetInfo();
        editorGui.mSingletonInfos[RenderWindow::Info::sType]      = GuiRenderWindow::GetInfo();
        editorGui.mSingletonInfos[SceneWindow::Info::sType]       = GuiSceneWindow::GetInfo();
        editorGui.mSingletonInfos[SingletonsWindow::Info::sType]  = GuiSingletonsWindow::GetInfo();
        editorGui.mSingletonInfos[TerrainWindow::Info::sType]     = GuiTerrainWindow::GetInfo();
        editorGui.mSingletonInfos[UnitTestsWindow::Info::sType]   = GuiUnitTestsWindow::GetInfo();

        //network
        editorGui.mSingletonInfos[HostManager::Info::sType]      = GuiHostManager::GetInfo();
        editorGui.mSingletonInfos[LinkingContext::Info::sType]   = GuiLinkingContext::GetInfo();
        editorGui.mSingletonInfos[ServerConnection::Info::sType] = GuiServerConnection::GetInfo();
        editorGui.mSingletonInfos[SpawnManager::Info::sType]     = GuiSpawnManager::GetInfo();
        editorGui.mSingletonInfos[Time::Info::sType]             = GuiTime::GetInfo();

        editorGui.mComponentInfos[ClientConnection::Info::sType]           = GuiEntityClientConnection::GetInfo();
        editorGui.mComponentInfos[ClientGameData::Info::sType]             = GuiClientGameData::GetInfo();
        editorGui.mComponentInfos[ClientReplication::Info::sType]          = GuiClientReplication::GetInfo();
        editorGui.mComponentInfos[ClientRollback::Info::sType]             = GuiClientRollback::GetInfo();
        editorGui.mComponentInfos[ClientRPC::Info::sType]                  = GuiClientRPC::GetInfo();
        editorGui.mComponentInfos[EntityReplication::Info::sType]          = GuiEntityReplication::GetInfo();
        editorGui.mComponentInfos[HostConnection::Info::sType]             = GuiHostConnection::GetInfo();
        editorGui.mComponentInfos[HostGameData::Info::sType]               = GuiHostGameData::GetInfo();
        editorGui.mComponentInfos[HostPersistentHandle::Info::sType]       = GuiHostPersistentHandle::GetInfo();
        editorGui.mComponentInfos[HostReplication::Info::sType]            = GuiHostReplication::GetInfo();
        editorGui.mComponentInfos[LinkingContextUnregisterer::Info::sType] = GuiLinkingContextUnregisterer::GetInfo();
        editorGui.mComponentInfos[ReliabilityLayer::Info::sType]           = GuiReliabilityLayer::GetInfo();

        // scene
        editorGui.mSingletonInfos[Application::Info::sType]     = GuiApplication::GetInfo();
        editorGui.mSingletonInfos[Mouse::Info::sType]           = GuiMouse::GetInfo();
        editorGui.mSingletonInfos[RenderDebug::Info::sType]     = GuiRenderDebug::GetInfo();
        editorGui.mSingletonInfos[RenderResources::Info::sType] = GuiRenderResources::GetInfo();
        editorGui.mSingletonInfos[RenderWorld::Info::sType]     = GuiRenderWorld::GetInfo();
        editorGui.mSingletonInfos[Scene::Info::sType]           = GuiScene::GetInfo();
        editorGui.mSingletonInfos[ScenePointers::Info::sType]   = GuiScenePointers::GetInfo();
        editorGui.mSingletonInfos[SceneResources::Info::sType]  = GuiSceneResources::GetInfo();
        editorGui.mSingletonInfos[VoxelTerrain::Info::sType]    = GuiVoxelTerrain::GetInfo();

        editorGui.mComponentInfos[UIAlign::Info::sType]       = GuiUIAlign::GetInfo();
        editorGui.mComponentInfos[UIButton::Info::sType]      = GuiUIButton::GetInfo();
        editorGui.mComponentInfos[UILayout::Info::sType]      = GuiUILayout::GetInfo();
        editorGui.mComponentInfos[UIProgressBar::Info::sType] = GuiUIProgressBar::GetInfo();
        editorGui.mComponentInfos[UIRenderer::Info::sType]    = GuiUIRenderer::GetInfo();
        editorGui.mComponentInfos[UIText::Info::sType]        = GuiUIText::GetInfo();
        editorGui.mComponentInfos[UITransform::Info::sType]   = GuiUITransform::GetInfo();

        editorGui.mComponentInfos[Bounds::Info::sType]           = GuiBounds::GetInfo();
        editorGui.mComponentInfos[Camera::Info::sType]           = GuiCamera::GetInfo();
        editorGui.mComponentInfos[DirectionalLight::Info::sType] = GuiDirectionalLight::GetInfo();
        editorGui.mComponentInfos[ExpirationTime::Info::sType]   = GuiExpirationTime::GetInfo();
        editorGui.mComponentInfos[FollowTransform::Info::sType]  = GuiFollowTransform::GetInfo();
        editorGui.mComponentInfos[Material::Info::sType]         = GuiMaterial::GetInfo();
        editorGui.mComponentInfos[MeshRenderer::Info::sType]     = GuiMeshRenderer::GetInfo();
        editorGui.mComponentInfos[Particle::Info::sType]         = GuiParticle::GetInfo();
        editorGui.mComponentInfos[ParticleEmitter::Info::sType]  = GuiParticleEmitter::GetInfo();
        editorGui.mComponentInfos[PointLight::Info::sType]       = GuiPointLight::GetInfo();
        editorGui.mComponentInfos[SceneNode::Info::sType]        = GuiSceneNode::GetInfo();

        // fx physics
        editorGui.mComponentInfos[Rigidbody::Info::sType]      = GuiFxRigidbody::GetInfo();
        editorGui.mSingletonInfos[PhysicsWorld::Info::sType]   = GuiFxPhysicsWorld::GetInfo();
        editorGui.mComponentInfos[BoxCollider::Info::sType]    = GuiFxBoxCollider::GetInfo();
        editorGui.mComponentInfos[SphereCollider::Info::sType] = GuiFxSphereCollider::GetInfo();

        editorGui.mComponentInfos[Transform::Info::sType] = GuiFxTransform::GetInfo();
        editorGui.mComponentInfos[Scale::Info::sType]     = GuiFxScale::GetInfo();
    }
}