#include "fanEditorSettings.hpp"

#include <fstream>
#include "core/memory/fanSerializable.hpp"
#include "core/input/fanInputManager.hpp"
#include "core/input/fanInput.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanVector3.hpp"
#include "core/fanColor.hpp"

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
        EditorSettings& serializedValues = static_cast<EditorSettings&>( _singleton );
        serializedValues.mData = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorSettingsData::SaveJsonToDisk( Json& _json )
    {
        Debug::Log( "Saving editor settings to disk" );
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
    /*void SaveEditorColor( Json& _json, const char* _name, const Color& _color )
    {
        Serializable::SaveColor( _json[sColorsName], _name, _color );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool LoadEditorColor( const Json& _json, const char* _name, Color& _outColor )
    {
        const Json* token = Serializable::FindToken( _json, sColorsName );
        if( token != nullptr )
        {
            return Serializable::LoadColor( *token, _name, _outColor );
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void SaveEditorWindowVisibleState( Json& _json, const uint32_t _type, const bool _visible )
    {
        Serializable::SaveBool( _json[sToolWindowName], _name, _visible );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool LoadEditorWindowVisibleState( const Json& _json, const uint32_t _type, bool _outVisible )
    {
        const Json* token = Serializable::FindToken( _json, _type );
        if( token != nullptr )
        {
            return Serializable::LoadBool( *token, _name, _outVisible );
        }
        return false;
    }*/

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