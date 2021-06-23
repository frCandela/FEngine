#include "editor/gui/fanGuiResourcePtr.hpp"
#include "core/fanPath.hpp"
#include "core/resources/fanResources.hpp"
#include "editor/fanDragnDrop.hpp"
#include "editor/fanModals.hpp"

namespace ImGui
{
    namespace impl
    {
        //==================================================================================================================================================================================================
        //==================================================================================================================================================================================================
        bool FanResourcePtr( const char* _label, fan::ResourcePtrData& _resourcePtrData, const fan::EditorResourceInfo& _info, const uint32_t _resourceType )
        {
            ImGui::PushID( _label );
            bool returnValue = false;

            fan::Resource* const resource = _resourcePtrData.mHandle != nullptr && _resourcePtrData.mHandle->mResource != nullptr ? _resourcePtrData.mHandle->mResource : nullptr;
            const std::string name = resource == nullptr ? "null" : fan::Path::FileName( resource->mPath );

            // icon
            {
                if( ImGui::ButtonIcon( _info.mIcon, { 16, 16 } ) )
                {
                    _resourcePtrData.mHandle = nullptr;
                    _resourcePtrData.mGUID   = 0;
                    returnValue = true;
                }
            }

            ImGui::SameLine();
            // name button
            const std::string  modalName = std::string( "Find " + std::string( _info.mResourceName ) + " (" ) + _label + ")";
            static std::string sPathBuffer;
            const float        width     = 0.6f * ImGui::GetWindowWidth() - ImGui::GetCursorPosX() + 23;
            if( ImGui::Button( name.c_str(), ImVec2( width, 0.f ) ) )
            {
                ImGui::OpenPopup( modalName.c_str() );
                sPathBuffer = fan::Path::Normalize( resource == nullptr || resource->mPath.empty() ? std::string( _info.mDefaultPath ) : resource->mPath );
            }

            // Dragndrop source
            if( resource != nullptr )
            {
                ImGui::FanBeginDragDropSourceResource( _resourcePtrData, _resourceType );
            }

            // tooltip
            if( resource != nullptr )
            {
                if( ImGui::IsItemHovered() )
                {
                    ImGui::BeginTooltip();
                    if( _info.mDrawTooltip != nullptr )
                    {
                        ( *_info.mDrawTooltip )( *resource );
                    }
                    else
                    {
                        ImGui::Text( "%s", resource->mPath.c_str() );
                    }
                    ImGui::EndTooltip();
                }
            }

            // dragndrop
            fan::ResourcePtrData textureDrop = ImGui::FanBeginDragDropTargetResource( _resourceType );
            if( textureDrop.mHandle != nullptr )
            {
                _resourcePtrData = textureDrop;
                returnValue      = true;
            }

            // Right click = clear
            if( ImGui::IsItemClicked( 1 ) )
            {
                _resourcePtrData.mGUID   = 0;
                _resourcePtrData.mHandle = nullptr;
                returnValue = true;
            }

            // Modal set value
            if( ImGui::FanLoadFileModal( modalName.c_str(), *_info.mExtensions, sPathBuffer ) )
            {
                _resourcePtrData = _resourcePtrData.sResourceManager->Load( _resourceType, sPathBuffer );
                returnValue      = true;
            }

            ImGui::SameLine();
            ImGui::Text( "%s", _label );

            ImGui::PopID();
            return returnValue;
        }
    }
}
