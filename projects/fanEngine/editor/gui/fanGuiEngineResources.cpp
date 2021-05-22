#include "editor/gui/fanGuiRenderResources.hpp"
#include <sstream>
#include "core/fanPath.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanRenderGlobal.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"
#include "editor/fanImguiIcons.hpp"

namespace ImGui
{

    //==================================================================================================================================================================================================
    // Draws a ImGui widget for displaying a TexturePtr
    // Returns true if the value (TexturePtr) was edited
    //==================================================================================================================================================================================================
    bool FanTexturePtr( const char* _label, fan::TexturePtr& _ptr )
    {
        bool returnValue = false;

        fan::Texture* texture = *_ptr;
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
        ImGui::FanBeginDragDropSourceTexture( texture );

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
        fan::Texture* textureDrop = ImGui::FanBeginDragDropTargetTexture();
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
            _ptr.Init( sPathBuffer );
            _ptr.mData.Resolve();
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
    bool FanMeshPtr( const char* _label, fan::MeshPtr& _ptr )
    {
        bool returnValue = false;

        fan::Mesh* mesh = *_ptr;
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
        ImGui::FanBeginDragDropSourceMesh( mesh );

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
        fan::Mesh* meshDrop = ImGui::FanBeginDragDropTargetMesh();
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
            _ptr.Init( sPathBuffer );
            _ptr.mData.Resolve();
            returnValue = true;
        }

        ImGui::SameLine();
        ImGui::Text( _label );

        return returnValue;
    }
}
