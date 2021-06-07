#include "editor/gui/fanGuiResourcePtr.hpp"
#include <sstream>
#include "core/fanPath.hpp"
#include "core/resources/fanResources.hpp"
#include "engine/resources/fanFont.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanSkinnedMesh.hpp"
#include "render/fanRenderGlobal.hpp"
#include "editor/fanImguiIcons.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"

namespace ImGui
{

    //==================================================================================================================================================================================================
    // Draws a ImGui widget for displaying a TexturePtr
    // Returns true if the value (TexturePtr) was edited
    //==================================================================================================================================================================================================
    bool FanTexturePtr( const char* _label, fan::ResourcePtr<fan::Texture>& _ptr )
    {
        bool returnValue = false;

        fan::Texture* texture = _ptr;
        const std::string name = texture == nullptr ? "null" : fan::Path::FileName( texture->mPath );

        // Set button icon & modal
        const std::string  modalName   = std::string( "Find texture (" ) + _label + ")";
        static std::string sPathBuffer = fan::Path::Normalize( "/" );
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Image16, { 16, 16 } ) )
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
                sPathBuffer = fan::Path::Normalize( fan::RenderGlobal::sContentPath );
            }
        }
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourceTexture( _ptr );

        // tooltip
        if( texture != nullptr )
        {
            if( ImGui::IsItemHovered() )
            {
                ImGui::BeginTooltip();

                // path
                ImGui::Text( texture->mPath.c_str() );

                // size
                std::stringstream ss;
                ss << texture->mExtent.width << " x " << texture->mExtent.height
                        << " x " << texture->mLayerCount;
                ImGui::Text( ss.str().c_str() );

                ImGui::EndTooltip();
            }
        }

        // dragndrop
        fan::ResourcePtr<fan::Texture> textureDrop = ImGui::FanBeginDragDropTargetTexture();
        if( textureDrop )
        {
            _ptr        = textureDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr        = nullptr;
            returnValue = true;
        }

        // Modal set value
        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sImagesExtensions, sPathBuffer ) )
        {
            _ptr        = _ptr.mData.sResourceManager->Load<fan::Texture>( sPathBuffer );
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }

    //==================================================================================================================================================================================================
    // Draws a ImGui widget for displaying a MeshPtr
    // Returns true if the value (MeshPtr) was edited
    //==================================================================================================================================================================================================
    bool FanMeshPtr( const char* _label, fan::ResourcePtr<fan::Mesh>& _ptr )
    {
        bool returnValue = false;

        fan::Mesh* mesh = _ptr;
        const std::string name = ( mesh == nullptr ) ? "null" : fan::Path::FileName( mesh->mPath );

        // Set button icon & modal
        const std::string  modalName = std::string( "Find mesh (" ) + _label + ")";
        static std::string sPathBuffer;
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Mesh16, { 16, 16 } ) )
            {
                _ptr        = nullptr;
                returnValue = true;
            }
        }
        ImGui::PopID();
        ImGui::SameLine();

        // name button
        const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
        if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
        {
            ImGui::OpenPopup( modalName.c_str() );
            if( sPathBuffer.empty() )
            {
                sPathBuffer = fan::Path::Normalize( fan::RenderGlobal::sModelsPath );
            }
        }
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourceMesh( _ptr );

        // tooltip
        if( mesh != nullptr && ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text( mesh->mPath.c_str() );
            for( fan::SubMesh& subMesh : mesh->mSubMeshes )
            {
                ImGui::Text( "%d triangles", (int)subMesh.mIndices.size() / 3 );
            }
            ImGui::EndTooltip();
        }

        // dragndrop
        fan::ResourcePtr<fan::Mesh> meshDrop = ImGui::FanBeginDragDropTargetMesh();
        if( meshDrop )
        {
            _ptr        = meshDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr        = nullptr;
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sMeshExtensions, sPathBuffer ) )
        {
            _ptr        = _ptr.mData.sResourceManager->Load<fan::Mesh>( sPathBuffer );
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanMeshSkinnedPtr( const char* _label, fan::ResourcePtr<fan::SkinnedMesh>& _ptr )
    {
        bool returnValue = false;

        fan::SkinnedMesh* mesh = _ptr;
        const std::string name = ( mesh == nullptr ) ? "null" : fan::Path::FileName( mesh->mPath );

        // Set button icon & modal
        const std::string  modalName = std::string( "Find skinned mesh (" ) + _label + ")";
        static std::string sPathBuffer;
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Mesh16, { 16, 16 } ) )
            {
                _ptr        = nullptr;
                returnValue = true;
            }
        }
        ImGui::PopID();
        ImGui::SameLine();

        // name button
        const float width = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 8;
        if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
        {
            ImGui::OpenPopup( modalName.c_str() );
            if( sPathBuffer.empty() )
            {
                sPathBuffer = fan::Path::Normalize( fan::RenderGlobal::sModelsPath );
            }
        }
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourceMeshSkinned( _ptr );

        // tooltip
        if( mesh != nullptr && ImGui::IsItemHovered() )
        {
            ImGui::BeginTooltip();
            ImGui::Text( mesh->mPath.c_str() );
            for( fan::SubSkinnedMesh& subMesh : mesh->mSubMeshes )
            {
                ImGui::Text( "%d triangles", (int)subMesh.mIndices.size() / 3 );
            }
            ImGui::EndTooltip();
        }

        // dragndrop
        fan::ResourcePtr<fan::SkinnedMesh> meshDrop = ImGui::FanBeginDragDropTargetMeshSkinned();
        if( meshDrop )
        {
            _ptr        = meshDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr        = nullptr;
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sMeshExtensions, sPathBuffer ) )
        {
            _ptr        = _ptr.mData.sResourceManager->Load<fan::SkinnedMesh>( sPathBuffer );
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanPrefabPtr( const char* _label, fan::ResourcePtr<fan::Prefab>& _ptr )
    {
        bool returnValue = false;

        fan::Prefab* prefab = _ptr;
        const std::string name = ( prefab == nullptr ? "null" : fan::Path::FileName( prefab->mPath ) );

        // Set button icon & modal
        const std::string  modalName = std::string( "Find prefab (" ) + _label + ")";
        static std::string sPathBuffer;
        ImGui::PushID( _label );
        {
            if( ImGui::ButtonIcon( ImGui::IconType::Prefab16, { 16, 16 } ) )
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
                sPathBuffer = fan::Path::Normalize( fan::RenderGlobal::sPrefabsPath );
            }
        }
        ImGui::SameLine();
        ImGui::FanBeginDragDropSourcePrefab( _ptr );

        // tooltip
        if( prefab != nullptr )
        {
            ImGui::FanToolTip( prefab->mPath.c_str() );
        }

        // dragndrop
        fan::ResourcePtr<fan::Prefab> prefabDrop = ImGui::FanBeginDragDropTargetPrefab();
        if( prefabDrop )
        {
            _ptr        = prefabDrop;
            returnValue = true;
        }

        // Right click = clear
        if( ImGui::IsItemClicked( 1 ) )
        {
            _ptr        = nullptr;
            returnValue = true;
        }

        if( ImGui::FanLoadFileModal( modalName.c_str(), fan::RenderGlobal::sPrefabExtensions, sPathBuffer ) )
        {
            _ptr        = _ptr.mData.sResourceManager->Load<fan::Prefab>( sPathBuffer );
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( "%s", _label );

        return returnValue;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool FanFontPtr( const char* _label, fan::ResourcePtr<fan::Font>& _ptr )
    {
        bool returnValue = false;

        fan::Font* font = _ptr;
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
        ImGui::FanBeginDragDropSourceFont( _ptr );

        // tooltip
        if( font != nullptr )
        {
            ImGui::FanToolTip( font->mPath.c_str() );
        }

        // dragndrop
        fan::ResourcePtr<fan::Font> fontDrop = ImGui::FanBeginDragDropTargetFont();
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
            _ptr        = _ptr.mData.sResourceManager->Load<fan::Font>( sPathBuffer );
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }
}
