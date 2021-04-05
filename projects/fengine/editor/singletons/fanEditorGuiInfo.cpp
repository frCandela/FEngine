#include "editor/singletons/fanEditorGuiInfo.hpp"

// EDITOR
#include "editor/gui/editor/singletons/fanGuiEditorGuiInfo.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorCamera.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorCopyPaste.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorGizmos.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorGrid.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorPlayState.hpp"
#include "editor/gui/editor/singletons/fanGuiEditorSelection.hpp"

// NETWORK
#include "editor/gui/network/singletons/fanGuiHostManager.hpp"
#include "editor/gui/network/singletons/fanGuiLinkingContext.hpp"
#include "editor/gui/network/singletons/fanGuiServerConnection.hpp"
#include "editor/gui/network/singletons/fanGuiSpawnManager.hpp"
#include "editor/gui/network/singletons/fanGuiTime.hpp"

#include "editor/gui/network/components/fanGuiClientConnection.hpp"
#include "editor/gui/network/components/fanGuiClientGameData.hpp"
#include "editor/gui/network/components/fanGuiClientReplication.hpp"
#include "editor/gui/network/components/fanGuiClientRollback.hpp"
#include "editor/gui/network/components/fanGuiClientRPC.hpp"
#include "editor/gui/network/components/fanGuiEntityReplication.hpp"
#include "editor/gui/network/components/fanGuiHostConnection.hpp"
#include "editor/gui/network/components/fanGuiHostGameData.hpp"
#include "editor/gui/network/components/fanGuiHostPersistentHandle.hpp"
#include "editor/gui/network/components/fanGuiHostReplication.hpp"
#include "editor/gui/network/components/fanGuiLinkingContextUnregisterer.hpp"
#include "editor/gui/network/components/fanGuiReliabilityLayer.hpp"

// SCENE
#include "editor/gui/scene/singletons/fanGuiApplication.hpp"
#include "editor/gui/scene/singletons/fanGuiMouse.hpp"
#include "editor/gui/scene/singletons/fanGuiPhysicsWorld.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderDebug.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderResources.hpp"
#include "editor/gui/scene/singletons/fanGuiRenderWorld.hpp"
#include "editor/gui/scene/singletons/fanGuiScene.hpp"
#include "editor/gui/scene/singletons/fanGuiScenePointers.hpp"
#include "editor/gui/scene/singletons/fanGuiSceneResources.hpp"

#include "editor/gui/scene/components/ui/fanGuiUIAlign.hpp"
#include "editor/gui/scene/components/ui/fanGuiUIButton.hpp"
#include "editor/gui/scene/components/ui/fanGuiUILayout.hpp"
#include "editor/gui/scene/components/ui/fanGuiUIProgressBar.hpp"
#include "editor/gui/scene/components/ui/fanGuiUIRenderer.hpp"
#include "editor/gui/scene/components/ui/fanGuiUIText.hpp"
#include "editor/gui/scene/components/ui/fanGuiUITransform.hpp"

#include "editor/gui/scene/components/fanGuiBounds.hpp"
#include "editor/gui/scene/components/fanGuiBoxShape.hpp"
#include "editor/gui/scene/components/fanGuiCamera.hpp"
#include "editor/gui/scene/components/fanGuiDirectionalLight.hpp"
#include "editor/gui/scene/components/fanGuiExpirationTime.hpp"
#include "editor/gui/scene/components/fanGuiFollowTransform.hpp"
#include "editor/gui/scene/components/fanGuiMaterial.hpp"
#include "editor/gui/scene/components/fanGuiMeshRenderer.hpp"
#include "editor/gui/scene/components/fanGuiMotionState.hpp"
#include "editor/gui/scene/components/fanGuiParticle.hpp"
#include "editor/gui/scene/components/fanGuiParticleEmitter.hpp"
#include "editor/gui/scene/components/fanGuiPointLight.hpp"
#include "editor/gui/scene/components/fanGuiRigidbody.hpp"
#include "editor/gui/scene/components/fanGuiSceneNode.hpp"
#include "editor/gui/scene/components/fanGuiSphereShape.hpp"
#include "editor/gui/scene/components/fanGuiTransform.hpp"
#include "editor/gui/scene/components/fanGuiFxTransform.hpp"

// Fx physics
#include "editor/gui/scene/components/physics/fanGuiFxRigidbody.hpp"

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

        //editor
        editorGui.mSingletonInfos[ EditorGuiInfo::Info::sType ]      = GuiEditorGuiInfo::GetInfo();
        editorGui.mSingletonInfos[ EditorCamera::Info::sType ]      = GuiEditorCamera::GetInfo();
        editorGui.mSingletonInfos[ EditorCopyPaste::Info::sType ]   = GuiEditorCopyPaste::GetInfo();
        editorGui.mSingletonInfos[ EditorGizmos::Info::sType ]      = GuiEditorGizmos::GetInfo();
        editorGui.mSingletonInfos[ EditorGrid::Info::sType ]        = GuiEditorGrid::GetInfo();
        editorGui.mSingletonInfos[ EditorPlayState::Info::sType ]   = GuiEditorPlayState::GetInfo();
        editorGui.mSingletonInfos[ EditorSelection::Info::sType ]   = GuiEditorSelection::GetInfo();

        //network
        editorGui.mSingletonInfos[ HostManager::Info::sType ]       = GuiHostManager::GetInfo();
        editorGui.mSingletonInfos[ LinkingContext::Info::sType ]    = GuiLinkingContext::GetInfo();
        editorGui.mSingletonInfos[ ServerConnection::Info::sType ]  = GuiServerConnection::GetInfo();
        editorGui.mSingletonInfos[ SpawnManager::Info::sType ]      = GuiSpawnManager::GetInfo();
        editorGui.mSingletonInfos[ Time::Info::sType ]              = GuiTime::GetInfo();

        editorGui.mComponentInfos[ ClientConnection::Info::sType ]          = GuiEntityClientConnection::GetInfo();
        editorGui.mComponentInfos[ ClientGameData::Info::sType ]            = GuiClientGameData::GetInfo();
        editorGui.mComponentInfos[ ClientReplication::Info::sType ]         = GuiClientReplication::GetInfo();
        editorGui.mComponentInfos[ ClientRollback::Info::sType ]            = GuiClientRollback::GetInfo();
        editorGui.mComponentInfos[ ClientRPC::Info::sType ]                 = GuiClientRPC::GetInfo();
        editorGui.mComponentInfos[ EntityReplication::Info::sType ]         = GuiEntityReplication::GetInfo();
        editorGui.mComponentInfos[ HostConnection::Info::sType ]            = GuiHostConnection::GetInfo();
        editorGui.mComponentInfos[ HostGameData::Info::sType ]              = GuiHostGameData::GetInfo();
        editorGui.mComponentInfos[ HostPersistentHandle::Info::sType ]      = GuiHostPersistentHandle::GetInfo();
        editorGui.mComponentInfos[ HostReplication::Info::sType ]           = GuiHostReplication::GetInfo();
        editorGui.mComponentInfos[ LinkingContextUnregisterer::Info::sType ]= GuiLinkingContextUnregisterer::GetInfo();
        editorGui.mComponentInfos[ ReliabilityLayer::Info::sType ]          = GuiReliabilityLayer::GetInfo();

        // scene
        editorGui.mSingletonInfos[ Application::Info::sType ]       = GuiApplication::GetInfo();
        editorGui.mSingletonInfos[ Mouse::Info::sType ]             = GuiMouse::GetInfo();
        editorGui.mSingletonInfos[ PhysicsWorld::Info::sType ]      = GuiPhysicsWorld::GetInfo();
        editorGui.mSingletonInfos[ RenderDebug::Info::sType ]       = GuiRenderDebug::GetInfo();
        editorGui.mSingletonInfos[ RenderResources::Info::sType ]   = GuiRenderResources::GetInfo();
        editorGui.mSingletonInfos[ RenderWorld::Info::sType ]       = GuiRenderWorld::GetInfo();
        editorGui.mSingletonInfos[ Scene::Info::sType ]             = GuiScene::GetInfo();
        editorGui.mSingletonInfos[ ScenePointers::Info::sType ]     = GuiScenePointers::GetInfo();
        editorGui.mSingletonInfos[ SceneResources::Info::sType ]    = GuiSceneResources::GetInfo();

        editorGui.mComponentInfos[ UIAlign::Info::sType ]       = GuiUIAlign::GetInfo();
        editorGui.mComponentInfos[ UIButton::Info::sType ]      = GuiUIButton::GetInfo();
        editorGui.mComponentInfos[ UILayout::Info::sType ]      = GuiUILayout::GetInfo();
        editorGui.mComponentInfos[ UIProgressBar::Info::sType ] = GuiUIProgressBar::GetInfo();
        editorGui.mComponentInfos[ UIRenderer::Info::sType ]    = GuiUIRenderer::GetInfo();
        editorGui.mComponentInfos[ UIText::Info::sType ]        = GuiUIText::GetInfo();
        editorGui.mComponentInfos[ UITransform::Info::sType ]   = GuiUITransform::GetInfo();

        editorGui.mComponentInfos[ Bounds::Info::sType ]   = GuiBounds::GetInfo();
        editorGui.mComponentInfos[ BoxShape::Info::sType ]   = GuiBoxShape::GetInfo();
        editorGui.mComponentInfos[ Camera::Info::sType ]   = GuiCamera::GetInfo();
        editorGui.mComponentInfos[ DirectionalLight::Info::sType ]   = GuiDirectionalLight::GetInfo();
        editorGui.mComponentInfos[ ExpirationTime::Info::sType ]   = GuiExpirationTime::GetInfo();
        editorGui.mComponentInfos[ FollowTransform::Info::sType ]   = GuiFollowTransform::GetInfo();
        editorGui.mComponentInfos[ Material::Info::sType ]   = GuiMaterial::GetInfo();
        editorGui.mComponentInfos[ MeshRenderer::Info::sType ]   = GuiMeshRenderer::GetInfo();
        editorGui.mComponentInfos[ MotionState::Info::sType ]   = GuiMotionState::GetInfo();
        editorGui.mComponentInfos[ Particle::Info::sType ]   = GuiParticle::GetInfo();
        editorGui.mComponentInfos[ ParticleEmitter::Info::sType ]   = GuiParticleEmitter::GetInfo();
        editorGui.mComponentInfos[ PointLight::Info::sType ]   = GuiPointLight::GetInfo();
        editorGui.mComponentInfos[ Rigidbody::Info::sType ]   = GuiRigidbody::GetInfo();
        editorGui.mComponentInfos[ SceneNode::Info::sType ]   = GuiSceneNode::GetInfo();
        editorGui.mComponentInfos[ SphereShape::Info::sType ]   = GuiSphereShape::GetInfo();
        editorGui.mComponentInfos[ Transform::Info::sType ]   = GuiTransform::GetInfo();

        editorGui.mComponentInfos[ FxRigidbody::Info::sType ]   = GuiFxRigidbody::GetInfo();

        editorGui.mComponentInfos[ FxTransform::Info::sType ]   = GuiFxTransform::GetInfo();
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
