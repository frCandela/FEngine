#include "editor/gui/fanGuiSceneResourcePtr.hpp"

#include "core/fanPath.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/fanDragnDrop.hpp"
#include "render/fanRenderGlobal.hpp"
#include "editor/singletons/fanEditorGuiInfo.hpp"
#include "editor/fanModals.hpp"


namespace ImGui
{
    //========================================================================================================
    // returns true if the component pointer changed value
    //========================================================================================================
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr )
    {
        fan::EcsWorld& world = *_ptr.mWorld;
        bool returnValue = false;

        const fan::EcsComponentInfo& info = world.GetComponentInfo( _ptr.mType );

        // create button title
        std::string name;
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
        const fan::EditorGuiInfo& gui = world.GetSingleton<fan::EditorGuiInfo>();
        const fan::GuiComponentInfo& guiInfo = gui.GetComponentInfo( info.mType );
        if (ImGui::ButtonIcon( guiInfo.mIcon, { 16, 16 } ))
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
        if (ImGui::IsItemClicked( 1 ))
        {
            _ptr.Clear();
            returnValue = true;
        }

        // label
        ImGui::Text( _label );

        return returnValue;
    }

    //========================================================================================================
    //========================================================================================================
    bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr )
    {
        bool returnValue = false;

        fan::Prefab* prefab = *_ptr;
        const std::string name = ( prefab == nullptr ? "null" : fan::Path::FileName( prefab->mPath ) );

        // Set button icon & modal
        const std::string modalName = std::string( "Find prefab (" ) + _label + ")";
        static std::string m_pathBuffer;
        bool openModal = false;
        ImGui::PushID( _label );
        {
            if ( ImGui::ButtonIcon( ImGui::IconType::Prefab16, { 16, 16 } ) )
            {
                openModal = true;
            }
        } ImGui::PopID();
        if ( openModal )
        {
            ImGui::OpenPopup( modalName.c_str() );
            m_pathBuffer = "content/prefab";
        }
        ImGui::SameLine();

        // name button
        const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
        ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ); ImGui::SameLine();
        ImGui::FanBeginDragDropSourcePrefab( prefab );

        // tooltip
        if ( prefab != nullptr )
        {
            ImGui::FanToolTip( prefab->mPath.c_str() );
        }

        // dragndrop
        fan::Prefab* prefabDrop = ImGui::FanBeginDragDropTargetPrefab();
        if ( prefabDrop )
        {
            _ptr.Set( prefabDrop );
            returnValue = true;
        }

        // Right click = clear
        if ( ImGui::IsItemClicked( 1 ) )
        {
            _ptr.Set( nullptr );
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sPrefabExtensions, m_pathBuffer ) )
        {
            _ptr.Init( m_pathBuffer );
            _ptr.Resolve();
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }
}