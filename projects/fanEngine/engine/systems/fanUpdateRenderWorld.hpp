#include "core/ecs/fanEcsSystem.hpp"
#include "engine/singletons/fanRenderWorld.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanSkinnedMeshRenderer.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanMaterial.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIRenderer.hpp"
#include "engine/components/fanPointLight.hpp"
#include "engine/components/fanDirectionalLight.hpp"
#include "engine/components/fanScale.hpp"
#include "engine/fanEngineTags.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Update the render world rendered meshes
    //==================================================================================================================================================================================================
    struct SUpdateRenderWorldModels : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>() | _world.GetSignature<Material>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, RenderWorld& _renderWorld )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            auto materialIt     = _view.begin<Material>();
            // get all mesh and adds them to the render world
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt, ++materialIt )
            {
                MeshRenderer& meshRenderer = *meshRendererIt;
                Transform   & transform    = *transformIt;
                Material    & material     = *materialIt;

                if( meshRenderer.mMesh != nullptr && !meshRenderer.mMesh->Empty() )
                {
                    const EcsEntity entity = transformIt.GetEntity();
                    const Vector3   scale  = _world.HasComponent<Scale>( entity ) ? _world.GetComponent<Scale>( entity ).mScale : Vector3::sOne;

                    for( int i = 0; i < meshRenderer.mMesh->mSubMeshes.size(); i++ )
                    {
                        SubMesh& subMesh = meshRenderer.mMesh->mSubMeshes[i];

                        // drawMesh data;
                        RenderDataModel data;
                        data.mMesh         = &subMesh;
                        data.mModelMatrix  = transform.GetModelMatrix( scale );
                        data.mNormalMatrix = transform.GetNormalMatrix( scale );

                        SubMaterial& subMaterial = i < material.mMaterials.size() ? material.mMaterials[i] : material.mMaterials[material.mMaterials.size() - 1];
                        data.mTexture   = subMaterial.mTexture;
                        data.mColor     = subMaterial.mColor.ToGLM();
                        data.mShininess = subMaterial.mShininess;

                        _renderWorld.mModels.push_back( data );
                    }
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Update the render world rendered meshes
    //==================================================================================================================================================================================================
    struct SUpdateRenderWorldModelsSkinned : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<SkinnedMeshRenderer>() | _world.GetSignature<Transform>() | _world.GetSignature<Material>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, RenderWorld& _renderWorld )
        {
            auto meshRendererIt = _view.begin<SkinnedMeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            auto materialIt     = _view.begin<Material>();
            // get all mesh and adds them to the render world
            for( ; meshRendererIt != _view.end<SkinnedMeshRenderer>(); ++meshRendererIt, ++transformIt, ++materialIt )
            {
                SkinnedMeshRenderer& meshRenderer = *meshRendererIt;
                Transform          & transform    = *transformIt;
                Material           & material     = *materialIt;

                if( meshRenderer.mMesh != nullptr && !meshRenderer.mMesh->Empty() )
                {
                    const EcsEntity entity = transformIt.GetEntity();
                    const Vector3   scale  = _world.HasComponent<Scale>( entity ) ? _world.GetComponent<Scale>( entity ).mScale : Vector3::sOne;

                    for( int i = 0; i < meshRenderer.mMesh->mSubMeshes.size(); i++ )
                    {
                        SubSkinnedMesh& subMesh = meshRenderer.mMesh->mSubMeshes[i];

                        // drawMesh data;
                        RenderDataSkinnedModel data;
                        data.mMesh         = &subMesh;
                        data.mSkeleton     = &meshRenderer.mMesh->mSkeleton;
                        data.mModelMatrix  = transform.GetModelMatrix( scale );
                        data.mNormalMatrix = transform.GetNormalMatrix( scale );
                        memcpy( data.mOffsetMatrix, meshRenderer.mOffsetMatrix, meshRenderer.mMesh->mSkeleton.mNumBones * sizeof( Matrix4 ) );

                        SubMaterial& subMaterial = i < material.mMaterials.size() ? material.mMaterials[i] : material.mMaterials[material.mMaterials.size() - 1];
                        data.mTexture   = subMaterial.mTexture;
                        data.mColor     = subMaterial.mColor.ToGLM();
                        data.mShininess = subMaterial.mShininess;

                        _renderWorld.mSkinnedModels.push_back( data );
                    }
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Get all ui mesh and adds them to the render world
    //==================================================================================================================================================================================================
    struct SUpdateRenderWorldUI : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UIRenderer>()
                   | _world.GetSignature<UITransform>()
                   | _world.GetSignature<TagVisible>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
            renderWorld.mUIModels.clear();

            auto rendererIt  = _view.begin<UIRenderer>();
            auto transformIt = _view.begin<UITransform>();

            for( ; rendererIt != _view.end<UIRenderer>(); ++rendererIt, ++transformIt )
            {
                UIRenderer       & renderer  = *rendererIt;
                const UITransform& transform = *transformIt;

                if( renderer.mTexture == nullptr || renderer.mMesh2D == nullptr )
                {
                    continue;
                }

                const glm::vec2 textureSize = glm::vec2( renderer.mTexture != nullptr ? glm::ivec2( renderer.mTexture->mExtent.width, renderer.mTexture->mExtent.height ) : glm::ivec2 { 1, 1 } );

                RenderDataMesh2D data;
                data.mMesh     = renderer.mMesh2D;
                data.mPosition = glm::vec2( transform.mPosition ) / renderWorld.mTargetSize * 2.f - glm::vec2( 1.f, 1.f );
                data.mScale    = glm::vec2( transform.mSize ) / renderWorld.mTargetSize;
                data.mUvOffset = glm::vec2( renderer.mUvOffset ) / textureSize;
                data.mUvScale  = glm::vec2( renderer.mTiling );
                data.mColor    = renderer.mColor.ToGLM();
                data.mTexture  = renderer.mTexture;
                data.mDepth    = renderer.mDepth;
                renderWorld.mUIModels.push_back( data );
            }

            auto sortFunc = []( RenderDataMesh2D& _a, RenderDataMesh2D& _b )
            {
                return _a.mDepth > _b.mDepth;
            };
            std::sort( renderWorld.mUIModels.begin(), renderWorld.mUIModels.end(), sortFunc );
        }
    };

    //==================================================================================================================================================================================================
    // Update the render world point lights
    //==================================================================================================================================================================================================
    struct SUpdateRenderWorldPointLights : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>()
                   | _world.GetSignature<PointLight>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
            renderWorld.mPointLights.clear();

            auto transformIt = _view.begin<Transform>();
            auto lightIt     = _view.begin<PointLight>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
            {
                const Transform& transform = *transformIt;
                PointLight     & light     = *lightIt;

                UniformPointLight pointLight;
                pointLight.mPosition  = glm::vec4( transform.mPosition.ToGlm(), 1.f );
                pointLight.mDiffuse   = light.mDiffuse.ToGLM();
                pointLight.mSpecular  = light.mSpecular.ToGLM();
                pointLight.mAmbiant   = light.mAmbiant.ToGLM();
                pointLight.mConstant  = light.mAttenuation[PointLight::Constant].ToFloat();
                pointLight.mLinear    = light.mAttenuation[PointLight::Linear].ToFloat();
                pointLight.mQuadratic = light.mAttenuation[PointLight::Quadratic].ToFloat();

                renderWorld.mPointLights.push_back( pointLight );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Update the render world directional lights
    //==================================================================================================================================================================================================
    struct SUpdateRenderWorldDirectionalLights : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>()
                   | _world.GetSignature<DirectionalLight>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();
            renderWorld.mDirectionalLights.clear();

            auto transformIt = _view.begin<Transform>();
            auto lightIt     = _view.begin<DirectionalLight>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++lightIt )
            {
                const Transform & transform        = *transformIt;
                DirectionalLight& directionalLight = *lightIt;

                UniformDirectionalLight light;
                light.mDirection = glm::vec4( transform.Forward().ToGlm(), 1 );
                light.mAmbiant   = directionalLight.mAmbiant.ToGLM();
                light.mDiffuse   = directionalLight.mDiffuse.ToGLM();
                light.mSpecular  = directionalLight.mSpecular.ToGLM();

                renderWorld.mDirectionalLights.push_back( light );
            }
        }
    };
}