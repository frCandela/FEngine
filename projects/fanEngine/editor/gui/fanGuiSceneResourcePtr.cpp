#include "editor/gui/fanGuiSceneResourcePtr.hpp"

#include "core/fanPath.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/resources/fanFont.hpp"
#include "editor/singletons/fanEditorSettings.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"

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

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanPrefab( const char* _label, fan::ResourcePtr<fan::Prefab>& _ptr )
    {
        bool returnValue = false;

        fan::Prefab* prefab = *_ptr;
        const std::string name = ( prefab == nullptr ? "null" : fan::Path::FileName( prefab->mPath ) );

        // Set button icon & modal
        const std::string  modalName = std::string( "Find prefab (" ) + _label + ")";
        static std::string m_pathBuffer;
        bool               openModal = false;
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Prefab16, { 16, 16 } ) )
            {
                openModal = true;
            }
        }
        ImGui::PopID();
        if( openModal )
        {
            ImGui::OpenPopup( modalName.c_str() );
            m_pathBuffer = fan::Path::Normalize( "/" );
        }
        ImGui::SameLine();

        // name button
        const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
        ImGui::Button( name.c_str(), ImVec2( width, 0.f ) );
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourcePrefab( prefab );

        // tooltip
        if( prefab != nullptr )
        {
            ImGui::FanToolTip( prefab->mPath.c_str() );
        }

        // dragndrop
        fan::Prefab* prefabDrop = ImGui::FanBeginDragDropTargetPrefab();
        if( prefabDrop )
        {
            _ptr.mData.mResource = prefabDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr.mData.mResource = nullptr;
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sPrefabExtensions, m_pathBuffer ) )
        {
            _ptr.mData.mPath = m_pathBuffer;
            _ptr.mData.Resolve();
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanFont( const char* _label, fan::ResourcePtr<fan::Font>& _ptr )
    {
        bool returnValue = false;

        fan::Font* font = *_ptr;
        const std::string name = ( font == nullptr ) ? "null" : fan::Path::FileName( font->mPath );

        // Set button icon & modal
        const std::string  modalName = std::string( "Find font (" ) + _label + ")";
        static std::string sPathBuffer;
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Font16, { 16, 16 } ) )
            {
                _ptr        = nullptr;
                returnValue = true;
            }
        }
        ImGui::PopID();
        ImGui::SameLine();

        // name button
        const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 23;
        if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
        {
            ImGui::OpenPopup( modalName.c_str() );
            if( sPathBuffer.empty() )
            {
                sPathBuffer = fan::Path::Normalize( fan::RenderGlobal::sFontsPath );
            }
        }
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourceFont( font );

        // tooltip
        if( font != nullptr )
        {
            ImGui::FanToolTip( font->mPath.c_str() );
        }

        // dragndrop
        fan::Font* fontDrop = ImGui::FanBeginDragDropTargetFont();
        if( fontDrop )
        {
            _ptr        = fontDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr        = nullptr;
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sFontsExtensions, sPathBuffer ) )
        {
            _ptr.mData.mPath = sPathBuffer;
            _ptr.mData.Resolve();
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }
}