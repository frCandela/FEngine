#include "fanEditorSettings.hpp"

#include <fstream>
#include "core/memory/fanSerializable.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanVector3.hpp"
#include "core/fanColor.hpp"

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
#include "editor/gui/singletons/fanGuiRenderWorld.hpp"
#include "editor/gui/singletons/fanGuiScene.hpp"
#include "editor/gui/singletons/fanGuiScenePointers.hpp"
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
#include "editor/gui/components/fanGuiMeshSkinnedRenderer.hpp"
#include "editor/gui/components/fanGuiParticle.hpp"
#include "editor/gui/components/fanGuiParticleEmitter.hpp"
#include "editor/gui/components/fanGuiPointLight.hpp"
#include "editor/gui/components/fanGuiSceneNode.hpp"
#include "editor/gui/components/fanGuiScale.hpp"
#include "editor/gui/components/fanGuiAnimator.hpp"

#include "editor/gui/physics/fanGuiTransform.hpp"
#include "editor/gui/physics/fanGuiRigidbody.hpp"
#include "editor/gui/physics/fanGuiSphereCollider.hpp"
#include "editor/gui/physics/fanGuiBoxCollider.hpp"
#include "editor/gui/physics/fanGuiPhysicsWorld.hpp"

namespace fan
{
    static const char* sJsonPath           = "editor_data.json";
    static const char* sKeysBindingsName   = "key_bindings";
    static const char* sGroupsColorsName   = "groups_colors";
    static const char* sImguiColorsName    = "imgui_colors";
    static const char* sWindowsVisibleName = "windows_visible";
    static const char* sImGuiPrefix        = "imgui_";

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettings::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mFlags |= EcsSingletonFlags::InitOnce;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettings::Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
    {
        EditorSettings& settings = static_cast<EditorSettings&>( _singleton );
        settings.mData = nullptr;
        InitComponentInfos( settings.mComponentInfos );
        InitSingletonInfos( settings.mSingletonInfos );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::SaveJsonToDisk( Json& _json )
    {
        Debug::Log( "Saving editor settings" );
        std::ofstream outFile( sJsonPath );
        fanAssert( outFile.is_open() );
        outFile << _json;
        outFile.close();
    }

    void SetDefaultColors();

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::LoadSettingsFromJson( EditorSettingsData& _settings )
    {
        Input::Get().Manager().Load( _settings.mJson[sKeysBindingsName] );

        // tools windows visibility
        const Json* jsonToolWindows = Serializable::FindToken( _settings.mJson, sWindowsVisibleName );
        if( jsonToolWindows )
        {
            for( Json::const_iterator it = jsonToolWindows->begin(); it != jsonToolWindows->end(); ++it )
            {
                std::string    s       = it.key();
                const uint32_t type    = std::stoul( s );
                const bool     visible = it.value();
                _settings.mToolWindowsVisibility[type] = visible;
            }
        }

        // loads imgui colors
        SetDefaultColors();
        ImGuiStyle& style          = ImGui::GetStyle();
        const Json* jsonImGuiColor = Serializable::FindToken( _settings.mJson, sImguiColorsName );
        if( jsonImGuiColor )
        {
            for( int i = 0; i < ImGuiCol_COUNT; i++ )
            {
                std::string name = sImGuiPrefix + std::string( ImGui::GetStyleColorName( i ) );
                Color       color;
                if( Serializable::LoadColor( *jsonImGuiColor, name.c_str(), color ) )
                {
                    style.Colors[i] = color.ToImGui();
                }
            }
        }

        // loads groups colors
        const Json* jsonGroupColor = Serializable::FindToken( _settings.mJson, sGroupsColorsName );
        if( jsonGroupColor )
        {
            // loads groups colors
            for( int i = 0; i < GroupsColors::sCount; i++ )
            {
                std::string name = sImGuiPrefix + std::string( GetEngineGroupName( EngineGroups( i ) ) );
                Color       color;
                if( Serializable::LoadColor( *jsonGroupColor, name.c_str(), color ) )
                {
                    _settings.mGroupsColors.mColors[i] = color.ToImGui();
                }
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::SaveSettingsToJson( EditorSettingsData& _settings )
    {
        Input::Get().Manager().Save( _settings.mJson[sKeysBindingsName] );

        // tools windows visibility
        Json& jsonToolWindows = _settings.mJson[sWindowsVisibleName];
        for( auto pair : _settings.mToolWindowsVisibility )
        {
            std::string typeString = std::to_string( pair.first );
            jsonToolWindows[typeString] = pair.second;
        }

        // save imgui colors
        Json      & jsonImGuiColors = _settings.mJson[sImguiColorsName];
        ImGuiStyle& style           = ImGui::GetStyle();
        for( int i = 0; i < ImGuiCol_COUNT; i++ )
        {
            std::string name = sImGuiPrefix + std::string( ImGui::GetStyleColorName( i ) );
            Serializable::SaveColor( jsonImGuiColors, name.c_str(), Color( style.Colors[i] ) );
        }

        // save groups colors
        Json& jsonGroupsColors = _settings.mJson[sGroupsColorsName];
        for( int i = 0; i < GroupsColors::sCount; i++ )
        {
            std::string name = sImGuiPrefix + std::string( GetEngineGroupName( EngineGroups( i ) ) );
            Serializable::SaveColor( jsonGroupsColors, name.c_str(), Color( _settings.mGroupsColors.mColors[i] ) );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::LoadJsonFromDisk( Json& _json )
    {
        std::ifstream inFile( sJsonPath );
        if( inFile.good() == true )
        {
            inFile >> _json;
        }
        inFile.close();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::SaveWindowSizeAndPosition( Json& _json, const glm::ivec2& _size, const glm::ivec2& _position )
    {
        Serializable::SaveIVec2( _json, "renderer_extent", _size );
        Serializable::SaveIVec2( _json, "renderer_position", _position );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool EditorSettingsData::LoadWindowSizeAndPosition( const Json& _json, glm::ivec2& _outSize, glm::ivec2& _outPosition )
    {
        return Serializable::LoadIVec2( _json, "renderer_extent", _outSize ) &&
               Serializable::LoadIVec2( _json, "renderer_position", _outPosition );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettings::InitSingletonInfos( std::unordered_map<uint32_t, GuiSingletonInfo>& _singletonInfos )
    {
        _singletonInfos.clear();

        //editor
        _singletonInfos[EditorCamera::Info::sType]      = GuiEditorCamera::GetInfo();
        _singletonInfos[EditorCopyPaste::Info::sType]   = GuiEditorCopyPaste::GetInfo();
        _singletonInfos[EditorGizmos::Info::sType]      = GuiEditorGizmos::GetInfo();
        _singletonInfos[EditorGrid::Info::sType]        = GuiEditorGrid::GetInfo();
        _singletonInfos[EditorPlayState::Info::sType]   = GuiEditorPlayState::GetInfo();
        _singletonInfos[EditorSelection::Info::sType]   = GuiEditorSelection::GetInfo();
        _singletonInfos[EditorSettings::Info::sType]    = GuiEditorSettings::GetInfo();
        _singletonInfos[EditorMainMenuBar::Info::sType] = GuiEditorMainMenuBar::GetInfo();

        // editor windows
        _singletonInfos[ConsoleWindow::Info::sType]     = GuiConsoleWindow::GetInfo();
        _singletonInfos[EcsWindow::Info::sType]         = GuiEcsWindow::GetInfo();
        _singletonInfos[InspectorWindow::Info::sType]   = GuiInspectorWindow::GetInfo();
        _singletonInfos[PreferencesWindow::Info::sType] = GuiPreferencesWindow::GetInfo();
        _singletonInfos[ProfilerWindow::Info::sType]    = GuiProfilerWindow::GetInfo();
        _singletonInfos[GameViewWindow::Info::sType]    = GuiGameViewWindow::GetInfo();
        _singletonInfos[RenderWindow::Info::sType]      = GuiRenderWindow::GetInfo();
        _singletonInfos[SceneWindow::Info::sType]       = GuiSceneWindow::GetInfo();
        _singletonInfos[SingletonsWindow::Info::sType]  = GuiSingletonsWindow::GetInfo();
        _singletonInfos[TerrainWindow::Info::sType]     = GuiTerrainWindow::GetInfo();
        _singletonInfos[UnitTestsWindow::Info::sType]   = GuiUnitTestsWindow::GetInfo();

        //network
        _singletonInfos[HostManager::Info::sType]      = GuiHostManager::GetInfo();
        _singletonInfos[LinkingContext::Info::sType]   = GuiLinkingContext::GetInfo();
        _singletonInfos[ServerConnection::Info::sType] = GuiServerConnection::GetInfo();
        _singletonInfos[SpawnManager::Info::sType]     = GuiSpawnManager::GetInfo();
        _singletonInfos[Time::Info::sType]             = GuiTime::GetInfo();

        // engine
        _singletonInfos[Application::Info::sType]   = GuiApplication::GetInfo();
        _singletonInfos[Mouse::Info::sType]         = GuiMouse::GetInfo();
        _singletonInfos[RenderDebug::Info::sType]   = GuiRenderDebug::GetInfo();
        _singletonInfos[RenderWorld::Info::sType]   = GuiRenderWorld::GetInfo();
        _singletonInfos[Scene::Info::sType]         = GuiScene::GetInfo();
        _singletonInfos[ScenePointers::Info::sType] = GuiScenePointers::GetInfo();
        _singletonInfos[VoxelTerrain::Info::sType]  = GuiVoxelTerrain::GetInfo();

        // fx physics
        _singletonInfos[PhysicsWorld::Info::sType] = GuiPhysicsWorld::GetInfo();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettings::InitComponentInfos( std::unordered_map<uint32_t, GuiComponentInfo>& _componentInfos )
    {
        _componentInfos.clear();

        _componentInfos[ClientConnection::Info::sType]           = GuiEntityClientConnection::GetInfo();
        _componentInfos[ClientGameData::Info::sType]             = GuiClientGameData::GetInfo();
        _componentInfos[ClientReplication::Info::sType]          = GuiClientReplication::GetInfo();
        _componentInfos[ClientRollback::Info::sType]             = GuiClientRollback::GetInfo();
        _componentInfos[ClientRPC::Info::sType]                  = GuiClientRPC::GetInfo();
        _componentInfos[EntityReplication::Info::sType]          = GuiEntityReplication::GetInfo();
        _componentInfos[HostConnection::Info::sType]             = GuiHostConnection::GetInfo();
        _componentInfos[HostGameData::Info::sType]               = GuiHostGameData::GetInfo();
        _componentInfos[HostPersistentHandle::Info::sType]       = GuiHostPersistentHandle::GetInfo();
        _componentInfos[HostReplication::Info::sType]            = GuiHostReplication::GetInfo();
        _componentInfos[LinkingContextUnregisterer::Info::sType] = GuiLinkingContextUnregisterer::GetInfo();
        _componentInfos[ReliabilityLayer::Info::sType]           = GuiReliabilityLayer::GetInfo();

        _componentInfos[UIAlign::Info::sType]       = GuiUIAlign::GetInfo();
        _componentInfos[UIButton::Info::sType]      = GuiUIButton::GetInfo();
        _componentInfos[UILayout::Info::sType]      = GuiUILayout::GetInfo();
        _componentInfos[UIProgressBar::Info::sType] = GuiUIProgressBar::GetInfo();
        _componentInfos[UIRenderer::Info::sType]    = GuiUIRenderer::GetInfo();
        _componentInfos[UIText::Info::sType]        = GuiUIText::GetInfo();
        _componentInfos[UITransform::Info::sType]   = GuiUITransform::GetInfo();

        _componentInfos[Bounds::Info::sType]              = GuiBounds::GetInfo();
        _componentInfos[Camera::Info::sType]              = GuiCamera::GetInfo();
        _componentInfos[DirectionalLight::Info::sType]    = GuiDirectionalLight::GetInfo();
        _componentInfos[ExpirationTime::Info::sType]      = GuiExpirationTime::GetInfo();
        _componentInfos[FollowTransform::Info::sType]     = GuiFollowTransform::GetInfo();
        _componentInfos[Material::Info::sType]            = GuiMaterial::GetInfo();
        _componentInfos[MeshRenderer::Info::sType]        = GuiMeshRenderer::GetInfo();
        _componentInfos[SkinnedMeshRenderer::Info::sType] = GuiSkinnedMeshRenderer::GetInfo();
        _componentInfos[Particle::Info::sType]            = GuiParticle::GetInfo();
        _componentInfos[ParticleEmitter::Info::sType]     = GuiParticleEmitter::GetInfo();
        _componentInfos[PointLight::Info::sType]          = GuiPointLight::GetInfo();
        _componentInfos[SceneNode::Info::sType]           = GuiSceneNode::GetInfo();
        _componentInfos[Animator::Info::sType]            = GuiAnimator::GetInfo();

        _componentInfos[Rigidbody::Info::sType]      = GuiRigidbody::GetInfo();
        _componentInfos[BoxCollider::Info::sType]    = GuiBoxCollider::GetInfo();
        _componentInfos[SphereCollider::Info::sType] = GuiSphereCollider::GetInfo();
        _componentInfos[Transform::Info::sType]      = GuiTransform::GetInfo();
        _componentInfos[Scale::Info::sType]          = GuiScale::GetInfo();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SetDefaultColors()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        // code generated using PreferencesWindow::LogColorsCppInitCode
        style.Colors[ImGuiCol_Text]                  = { (float)0.635294, (float)0.635294, (float)0.635294, (float)1 };
        style.Colors[ImGuiCol_TextDisabled]          = { (float)0.5, (float)0.5, (float)0.5, (float)1 };
        style.Colors[ImGuiCol_WindowBg]              = { (float)0.131201, (float)0.131188, (float)0.131188, (float)1 };
        style.Colors[ImGuiCol_ChildBg]               = { (float)1, (float)1, (float)1, (float)0 };
        style.Colors[ImGuiCol_PopupBg]               = { (float)0.08, (float)0.08, (float)0.08, (float)0.94 };
        style.Colors[ImGuiCol_Border]                = { (float)0.172522, (float)0.172539, (float)0.172529, (float)1 };
        style.Colors[ImGuiCol_BorderShadow]          = { (float)0.0001, (float)9.999e-05, (float)9.999e-05, (float)0 };
        style.Colors[ImGuiCol_FrameBg]               = { (float)0.247059, (float)0.247059, (float)0.247059, (float)0.541176 };
        style.Colors[ImGuiCol_FrameBgHovered]        = { (float)0.509804, (float)0.509804, (float)0.509804, (float)0.4 };
        style.Colors[ImGuiCol_FrameBgActive]         = { (float)0.564706, (float)0.564706, (float)0.564706, (float)0.670588 };
        style.Colors[ImGuiCol_TitleBg]               = { (float)0.0941176, (float)0.0941176, (float)0.0941176, (float)1 };
        style.Colors[ImGuiCol_TitleBgActive]         = { (float)0.0950447, (float)0.0950352, (float)0.0950352, (float)1 };
        style.Colors[ImGuiCol_TitleBgCollapsed]      = { (float)0, (float)0, (float)0, (float)1 };
        style.Colors[ImGuiCol_MenuBarBg]             = { (float)0.14, (float)0.14, (float)0.14, (float)1 };
        style.Colors[ImGuiCol_ScrollbarBg]           = { (float)0.02, (float)0.02, (float)0.02, (float)0.53 };
        style.Colors[ImGuiCol_ScrollbarGrab]         = { (float)0.31, (float)0.31, (float)0.31, (float)1 };
        style.Colors[ImGuiCol_ScrollbarGrabHovered]  = { (float)0.41, (float)0.41, (float)0.41, (float)1 };
        style.Colors[ImGuiCol_ScrollbarGrabActive]   = { (float)0.51, (float)0.51, (float)0.51, (float)1 };
        style.Colors[ImGuiCol_CheckMark]             = { (float)0.403922, (float)1, (float)0.67451, (float)1 };
        style.Colors[ImGuiCol_SliderGrab]            = { (float)0.658824, (float)0.658824, (float)0.658824, (float)1 };
        style.Colors[ImGuiCol_SliderGrabActive]      = { (float)0.454902, (float)1, (float)0.756863, (float)1 };
        style.Colors[ImGuiCol_Button]                = { (float)0.537255, (float)0.537255, (float)0.537255, (float)0.4 };
        style.Colors[ImGuiCol_ButtonHovered]         = { (float)0.415686, (float)0.415686, (float)0.415686, (float)1 };
        style.Colors[ImGuiCol_ButtonActive]          = { (float)0.482353, (float)0.482353, (float)0.482353, (float)1 };
        style.Colors[ImGuiCol_Header]                = { (float)0.321569, (float)0.321569, (float)0.321569, (float)0.309804 };
        style.Colors[ImGuiCol_HeaderHovered]         = { (float)0.352941, (float)0.352941, (float)0.352941, (float)0.8 };
        style.Colors[ImGuiCol_HeaderActive]          = { (float)0.396078, (float)0.396078, (float)0.396078, (float)1 };
        style.Colors[ImGuiCol_Separator]             = { (float)0.439216, (float)0.439216, (float)0.439216, (float)0.501961 };
        style.Colors[ImGuiCol_SeparatorHovered]      = { (float)0.498039, (float)0.498039, (float)0.498039, (float)0.780392 };
        style.Colors[ImGuiCol_SeparatorActive]       = { (float)0.509804, (float)0.509804, (float)0.509804, (float)1 };
        style.Colors[ImGuiCol_ResizeGrip]            = { (float)0.360784, (float)0.360784, (float)0.360784, (float)0.25098 };
        style.Colors[ImGuiCol_ResizeGripHovered]     = { (float)0.478431, (float)0.478431, (float)0.478431, (float)0.670588 };
        style.Colors[ImGuiCol_ResizeGripActive]      = { (float)0.4, (float)0.4, (float)0.4, (float)0.94902 };
        style.Colors[ImGuiCol_Tab]                   = { (float)0.18, (float)0.35, (float)0.58, (float)0.862 };
        style.Colors[ImGuiCol_TabHovered]            = { (float)0.26, (float)0.59, (float)0.98, (float)0.8 };
        style.Colors[ImGuiCol_TabActive]             = { (float)0.2, (float)0.41, (float)0.68, (float)1 };
        style.Colors[ImGuiCol_TabUnfocused]          = { (float)0.068, (float)0.102, (float)0.148, (float)0.9724 };
        style.Colors[ImGuiCol_TabUnfocusedActive]    = { (float)0.136, (float)0.262, (float)0.424, (float)1 };
        style.Colors[ImGuiCol_DockingPreview]        = { (float)0.26, (float)0.59, (float)0.98, (float)0.7 };
        style.Colors[ImGuiCol_DockingEmptyBg]        = { (float)0.2, (float)0.2, (float)0.2, (float)1 };
        style.Colors[ImGuiCol_PlotLines]             = { (float)0.529412, (float)0.529412, (float)0.529412, (float)1 };
        style.Colors[ImGuiCol_PlotLinesHovered]      = { (float)1, (float)0.43, (float)0.35, (float)1 };
        style.Colors[ImGuiCol_PlotHistogram]         = { (float)0.9, (float)0.7, (float)0, (float)1 };
        style.Colors[ImGuiCol_PlotHistogramHovered]  = { (float)1, (float)0.6, (float)0, (float)1 };
        style.Colors[ImGuiCol_TextSelectedBg]        = { (float)0.26, (float)0.59, (float)0.98, (float)0.35 };
        style.Colors[ImGuiCol_DragDropTarget]        = { (float)1, (float)1, (float)0, (float)0.9 };
        style.Colors[ImGuiCol_NavHighlight]          = { (float)0.26, (float)0.59, (float)0.98, (float)1 };
        style.Colors[ImGuiCol_NavWindowingHighlight] = { (float)1, (float)1, (float)1, (float)0.7 };
        style.Colors[ImGuiCol_NavWindowingDimBg]     = { (float)0.8, (float)0.8, (float)0.8, (float)0.2 };
        style.Colors[ImGuiCol_ModalWindowDimBg]      = { (float)0.8, (float)0.8, (float)0.8, (float)0.35 };
    }
}