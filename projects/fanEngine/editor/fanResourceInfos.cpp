#include "editor/fanResourceInfos.hpp"

#include "render/resources/fanTexture.hpp"
#include "render/resources/fanMesh.hpp"
#include "engine/resources/fanFont.hpp"
#include "engine/resources/fanPrefab.hpp"
#include "render/resources/fanSkinnedMesh.hpp"
#include "render/resources/fanAnimation.hpp"

namespace fan
{

    const std::map<uint32_t, EditorResourceInfo> EditorResourceInfo::sResourceInfos
                                                         = {
                    //Texture
                    {
                            Texture::Info::sType,
                            {
                                    ImGui::IconType::Image16,
                                    fan::RenderGlobal::sImagesPath,
                                    "Texture",
                                    &fan::RenderGlobal::sImagesExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::Texture& texture = static_cast<const fan::Texture&>(_resource);
                                        ImGui::Text( "%s", texture.mPath.c_str() );
                                        ImGui::Text( "%d x %d x %d", texture.mExtent.width, texture.mExtent.height, texture.mLayerCount );
                                    }
                            }
                    },

                    //Mesh
                    {
                            Mesh::Info::sType,
                            {
                                    ImGui::IconType::Mesh16,
                                    fan::RenderGlobal::sModelsPath,
                                    "Mesh",
                                    &fan::RenderGlobal::sMeshExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::Mesh& mesh = static_cast<const fan::Mesh&>(_resource);
                                        ImGui::Text( "%s", mesh.mPath.c_str() );
                                        for( const fan::SubMesh& subMesh : mesh.mSubMeshes )
                                        {
                                            ImGui::Text( "%d triangles", (int)subMesh.mIndices.size() / 3 );
                                        }
                                    }
                            }
                    },
                    //SkinnedMesh
                    {
                            SkinnedMesh::Info::sType,
                            {
                                    ImGui::IconType::Mesh16,
                                    fan::RenderGlobal::sModelsPath,
                                    "Skinned Mesh",
                                    &fan::RenderGlobal::sMeshExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::SkinnedMesh& mesh = static_cast<const fan::SkinnedMesh&>(_resource);
                                        ImGui::Text( "%s", mesh.mPath.c_str() );
                                        ImGui::Text( "%d bones", (int)mesh.mSkeleton.mNumBones );
                                        for( const fan::SubSkinnedMesh& subMesh : mesh.mSubMeshes )
                                        {
                                            ImGui::Text( "%d triangles", (int)subMesh.mIndices.size() / 3 );
                                        }
                                    }
                            }
                    },
                    //Prefab
                    {
                            Prefab::Info::sType,
                            {
                                    ImGui::IconType::Prefab16,
                                    fan::RenderGlobal::sPrefabsPath,
                                    "Prefab",
                                    &fan::RenderGlobal::sPrefabExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::Prefab& prefab = static_cast<const fan::Prefab&>(_resource);
                                        ImGui::Text( "%s", prefab.mPath.c_str() );
                                    }
                            }
                    },
                    //Font
                    {
                            Font::Info::sType,
                            {
                                    ImGui::IconType::Font16,
                                    fan::RenderGlobal::sFontsPath,
                                    "Font",
                                    &fan::RenderGlobal::sFontsExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::Font& font = static_cast<const fan::Font&>(_resource);
                                        ImGui::Text( "%s", font.mPath.c_str() );
                                    }
                            }
                    },
                    //Animation
                    {
                            Animation::Info::sType,
                            {
                                    ImGui::IconType::Animator16,
                                    fan::RenderGlobal::sAnimationsPath,
                                    "Font",
                                    &fan::RenderGlobal::sAnimationsExtensions,
                                    []( const fan::Resource& _resource )
                                    {
                                        const fan::Animation& animation = static_cast<const fan::Animation&>(_resource);
                                        ImGui::Text( "%s", animation.mPath.c_str() );
                                        ImGui::Text( "%d bones", animation.mNumBones );
                                    }
                            }
                    },
            };
}