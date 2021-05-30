#include "editor/gui/fanGuiComponentPtr.hpp"

#include "core/fanPath.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanDragnDrop.hpp"

namespace ImGui
{
    //==================================================================================================================================================================================================
    // returns true if the component pointer changed value
    //==================================================================================================================================================================================================
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr )
    {
        fan::EcsWorld& world = *_ptr.mWorld;
        bool returnValue = false;

        const fan::EcsComponentInfo& info    = world.GetComponentInfo( _ptr.mType );

        // create button title
        std::string                name;
        if( _ptr.mHandle == 0 )
        {
            name = info.mName + " : NULL";
        }
        else
        {
            fan::SceneNode& node = world.GetComponent<fan::SceneNode>( world.GetEntity( _ptr.mHandle ) );
            name = info.mName + " : " + node.mName;
        }
        // icon
        const fan::EditorSettings   & settings = world.GetSingleton<fan::EditorSettings>();
        const fan::GuiComponentInfo& guiInfo   = settings.GetComponentInfo( info.mType );
        if( ImGui::ButtonIcon( guiInfo.mIcon, { 16, 16 } ) )
        {
            returnValue = true;
        }
        // dragndrop source for icon
        if( _ptr.mHandle != 0 )
        {
            fan::SceneNode& node = world.GetComponent<fan::SceneNode>( world.GetEntity( _ptr.mHandle ) );
            ImGui::FanBeginDragDropSourceComponent( world, node.mHandle, _ptr.mType );
        }
        // dragndrop target for icon
        ImGui::ComponentPayload payloadIcon = ImGui::FanBeginDragDropTargetComponent( world, _ptr.mType );

        ImGui::SameLine();

        // name button
        float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 23;
        if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
        {
            // @todo select target in the editor
        }
        ImGui::SameLine();

        // dragndrop source for button
        if( _ptr.mHandle != 0 )
        {
            ImGui::FanBeginDragDropSourceComponent( world, _ptr.mHandle, _ptr.mType );
        }

        // dragndrop target for button
        ImGui::ComponentPayload payloadButton = ImGui::FanBeginDragDropTargetComponent( world, _ptr.mType );
        if( payloadButton.mHandle != 0 || payloadIcon.mHandle != 0 )
        {
            ImGui::ComponentPayload& payload = ( payloadButton.mHandle != 0
                    ? payloadButton
                    : payloadIcon );
            _ptr.Create( payload.mHandle );
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr.Clear();
            returnValue = true;
        }

        // label
        ImGui::Text( _label );

        return returnValue;
    }
}