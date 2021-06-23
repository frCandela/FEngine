#include "editor/fanModals.hpp"

#include <sstream>
#include <core/fanDebug.hpp>
#include <core/fanPath.hpp>
#include <fanDisableWarnings.hpp>
#include "core/input/fanKeyboard.hpp"

WARNINGS_IMGUI_PUSH()
#include "imgui/imgui_internal.h"

WARNINGS_POP()

namespace ImGui
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool DragFixed( const char* label, fan::Fixed* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
    {
        float tmpV = v->ToFloat();
        if( ImGui::DragFloat( label, &tmpV, v_speed, v_min, v_max, format, flags ) )
        {
            *v = fan::Fixed::FromFloat( tmpV );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool DragFixed2( const char* label, fan::Fixed v[2], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
    {
        float tmpV[2] = { v[0].ToFloat(), v[1].ToFloat() };
        if( ImGui::DragFloat2( label, tmpV, v_speed, v_min, v_max, format, flags ) )
        {
            v[0] = fan::Fixed::FromFloat( tmpV[0] );
            v[1] = fan::Fixed::FromFloat( tmpV[1] );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool DragFixed3( const char* label, fan::Fixed v[3], float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
    {
        float tmpV[3] = { v[0].ToFloat(), v[1].ToFloat(), v[2].ToFloat() };
        if( ImGui::DragFloat3( label, tmpV, v_speed, v_min, v_max, format, flags ) )
        {
            v[0] = fan::Fixed::FromFloat( tmpV[0] );
            v[1] = fan::Fixed::FromFloat( tmpV[1] );
            v[2] = fan::Fixed::FromFloat( tmpV[2] );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool SliderFixed( const char* label, fan::Fixed* v, fan::Fixed v_min, fan::Fixed v_max, const char* format, ImGuiSliderFlags flags )
    {
        float tmpV = v->ToFloat();
        if( ImGui::SliderFloat( label, &tmpV, v_min.ToFloat(), v_max.ToFloat(), format, flags ) )
        {
            *v = fan::Fixed::FromFloat( tmpV );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanShowHelpMarker( const char* _desc )
    {
        ImGui::TextDisabled( "(?)" );
        FanToolTip( _desc );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void FanToolTip( const char* _desc )
    {
        if( ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos( ImGui::GetFontSize() * 35.0f );
            ImGui::TextUnformatted( _desc );
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool impl::FilesSelector( const std::set<std::string>& _extensionWhiteList, std::string& _path )
    {
        bool returnValue = false;

        ImGui::SetNextItemOpen( true );
        if( ImGui::TreeNode( "File Selection" ) )
        {
            // '.' Selectable to go to the parent
            if( _path != "." &&
                ImGui::Selectable( ".", false, ImGuiSelectableFlags_DontClosePopups | ImGuiSelectableFlags_AllowDoubleClick ) )
            {
                if( !fan::Path::IsRootDrive( _path ) )
                {
                    _path = fan::Path::Parent( _path );
                }
            }

            // Lists all directories
            const std::string dir = fan::Path::Directory( _path );
            for( const std::string& childPath : fan::Path::ListDirectory( dir ) )
            {
                std::string extension   = fan::Path::Extension( childPath );
                const bool  isFile      = _extensionWhiteList.find( extension ) != _extensionWhiteList.end();
                const bool  isDirectory = fan::Path::IsDirectory( childPath );
                // Filter
                if( isFile || isDirectory )
                {
                    const std::string filename = fan::Path::FileName( childPath );
                    if( isFile )
                    {
                        ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4)( ImColor( 1.0f, 0.712f, 0.0f ) ) );
                    }
                    if( ImGui::Selectable( filename.c_str(),
                                           false,
                                           ImGuiSelectableFlags_DontClosePopups |
                                           ImGuiSelectableFlags_AllowDoubleClick ) )
                    {
                        _path = childPath;
                        if( ImGui::IsMouseDoubleClicked( 0 ) && isFile )
                        {
                            returnValue = true;
                        }
                    }
                    if( isFile )
                    {
                        ImGui::PopStyleColor();
                    }
                }
            }

            ImGui::TreePop();
        }

        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanSaveFileModal( const char* _popupName, const std::set<std::string>& _extensionWhiteList, std::string& _outCurrentPath )
    {
        bool returnValue = false;

        static int extensionIndex = 0;

        ImGui::SetNextWindowSize( { 316, 410 } );
        if( ImGui::BeginPopupModal( _popupName ) )
        {
            // Files hierarchy
            bool itemDoubleClicked = false;
            ImGui::BeginChild( "load_scene_hierarchy", { 300, 300 }, true );
            {
                itemDoubleClicked = impl::FilesSelector( _extensionWhiteList, _outCurrentPath );
            }
            ImGui::EndChild();


            // Input name
            std::string filename   = fan::Path::IsDirectory( _outCurrentPath ) ? "new_scene" : fan::Path::FileName( _outCurrentPath );
            const int   bufferSize = 64;
            char        buffer[bufferSize];
            strcpy_s( buffer, filename.c_str() );

            if( ImGui::IsWindowAppearing() )
            {
                ImGui::SetKeyboardFocusHere();
                extensionIndex = 0;
            }
            bool enterPressed = false;
            if( ImGui::InputText( "name", buffer, IM_ARRAYSIZE( buffer ), ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
                enterPressed = true;
            }
            ImGui::SetItemDefaultFocus();


            // Format selection
            std::stringstream        extensions;
            std::vector<std::string> tmpExtensions;
            for( const std::string& extension : _extensionWhiteList )
            {
                extensions << extension << '\0';
                tmpExtensions.push_back( extension );
            }
            if( _extensionWhiteList.size() > 1 )
            {
                ImGui::Combo( "format", &extensionIndex, extensions.str().c_str() );
            }
            else
            {
                ImGui::TextDisabled( extensions.str().c_str() );
            }
            ImGui::Separator();

            // re format to path/.../filename.extension
            std::string stringBuffer = fan::Path::FileName( buffer );
            _outCurrentPath = fan::Path::Directory( _outCurrentPath );
            if( stringBuffer.length() > 0 )
            {
                _outCurrentPath.append( stringBuffer + "." + tmpExtensions[extensionIndex] );
            }

            // Ok Button
            if( itemDoubleClicked || ImGui::Button( "Ok" ) || enterPressed )
            {
                if( !fan::Path::IsDirectory( _outCurrentPath ) )
                {
                    ImGui::CloseCurrentPopup();
                    returnValue = true;
                }
            }

            // Cancel button
            ImGui::SameLine();
            if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanLoadFileModal( const char* _popupName, const std::set<std::string>& _extensionWhiteList, std::string& _path )
    {
        bool returnValue = false;

        ImGui::SetNextWindowSize( { 316, 400 } );

        if( ImGui::BeginPopupModal( _popupName, NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove ) )
        {
            static char overrideTextBuffer[64];
            ImGui::PushItemWidth( 220 );
            ImGui::InputText( "##override", overrideTextBuffer, 256 );
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if( ImGui::Button( "override" ) )
            {
                ImGui::CloseCurrentPopup();
                _path = overrideTextBuffer;
                fan::Debug::Log( _path );
                ImGui::EndPopup();
                return true;
            }

            ImGui::BeginChild( "files_region", { 300, 280 }, true );
            ImGui::Text( "placeholder" );
            bool itemDoubleClicked = impl::FilesSelector( _extensionWhiteList, _path );
            if( itemDoubleClicked )
            {
            }
            ImGui::EndChild();

            ImGui::Text( _path.c_str() );
            ImGui::Separator();

            if( itemDoubleClicked == true || ImGui::Button( "Ok" ) || ImGui::IsKeyPressed( GLFW_KEY_ENTER, false ) )
            {
                if( fan::Path::IsFile( _path ) )
                {
                    ImGui::CloseCurrentPopup();
                    returnValue = true;
                }
            }
            ImGui::SameLine();
            if( ImGui::Button( "Cancel" ) || ImGui::IsKeyPressed( GLFW_KEY_ESCAPE, false ) )
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PushReadOnly()
    {
        ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
        ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PopReadOnly()
    {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}
