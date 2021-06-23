#include "editor/fanResourceInfos.hpp"

#include "render/resources/fanTexture.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{

    const std::map<uint32_t, EditorResourceInfo> EditorResourceInfo::sResourceInfos =
                                                         {
                                                                 {
                                                                         //Texture
                                                                         Texture::Info::sType,
                                                                         {
                                                                                 ImGui::IconType::Image16,
                                                                                 fan::RenderGlobal::sImagesPath,
                                                                                 "Texture",
                                                                                 &fan::RenderGlobal::sImagesExtensions,
                                                                                 []( fan::Resource* _resource )
                                                                                 {
                                                                                     ImGui::Text( "%s", _resource->mPath.c_str() );
                                                                                     fan::Texture* texture = static_cast<fan::Texture*>(_resource);
                                                                                     ImGui::Text( "%d x %d x %d", texture->mExtent.width, texture->mExtent.height, texture->mLayerCount );
                                                                                 }
                                                                         }
                                                                 }
                                                         };
}