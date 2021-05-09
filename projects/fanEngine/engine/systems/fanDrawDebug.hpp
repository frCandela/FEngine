#include "core/ecs/fanEcsSystem.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIRenderer.hpp"
#include "engine/ui/fanUILayout.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/fanSceneTags.hpp"
#include "engine/physics/fanFxSphereCollider.hpp"
#include "engine/physics/fanFxBoxCollider.hpp"

namespace fan
{
    class EcsWorld;
    struct DirectionalLight;

    //==================================================================================================================================================================================================
    // Draw the bounds of all scene nodes
    //==================================================================================================================================================================================================
    struct SDrawDebugBounds : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt )
            {
                const Bounds& bounds = *boundsIt;
                _world.GetSingleton<RenderDebug>().DebugAABB( bounds.mAabb, Color::sRed );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw the normals of all models
    //==================================================================================================================================================================================================
    struct SDrawDebugNormals : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<FxTransform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<FxTransform>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                const FxTransform & transform    = *transformIt;

                if( *meshRenderer.mMesh != nullptr )
                {
                    const glm::mat4 modelMat  = transform.GetModelMatrix();
                    const glm::mat4 normalMat = transform.GetNormalMatrix();
                    const std::vector<uint32_t>& indices  = meshRenderer.mMesh->mIndices;
                    const std::vector<Vertex>  & vertices = meshRenderer.mMesh->mVertices;

                    for( int index = 0; index < (int)indices.size(); index++ )
                    {
                        const Vertex& vertex = vertices[indices[index]];
                        const Vector3 position = Vector3( modelMat * glm::vec4( vertex.mPos, 1.f ) );
                        const Vector3 normal   = Vector3( normalMat * glm::vec4( vertex.mNormal, 1.f ) );
                        RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
                        renderDebug.DebugLine( position, position + FIXED( 0.1 ) * normal, Color::sGreen );
                    }
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw all the models in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugWireframe : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<FxTransform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<FxTransform>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                const FxTransform & transform    = *transformIt;
                if( *meshRenderer.mMesh == nullptr ){ continue; }

                const glm::mat4 modelMat = transform.GetModelMatrix();
                const std::vector<uint32_t>& indices  = meshRenderer.mMesh->mIndices;
                const std::vector<Vertex>  & vertices = meshRenderer.mMesh->mVertices;

                for( int index = 0; index < (int)indices.size() / 3; index++ )
                {
                    const Vector3 v0 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 0]].mPos, 1.f ) );
                    const Vector3 v1 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 1]].mPos, 1.f ) );
                    const Vector3 v2 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 2]].mPos, 1.f ) );
                    _world.GetSingleton<RenderDebug>().DebugLine( v0, v1, Color::sYellow );
                    _world.GetSingleton<RenderDebug>().DebugLine( v1, v2, Color::sYellow );
                    _world.GetSingleton<RenderDebug>().DebugLine( v2, v0, Color::sYellow );
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw all the models convex hull in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugHull : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<FxTransform>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                const FxTransform & transform    = *transformIt;

                if( *meshRenderer.mMesh == nullptr ){ continue; }

                const ConvexHull& hull = meshRenderer.mMesh->mConvexHull;
                if( hull.mVertices.empty() ){ continue; }

                const glm::mat4 modelMat = transform.GetModelMatrix();

                Color         color     = Color::sCyan;
                for( unsigned polyIndex = 0; polyIndex < hull.mIndices.size() / 3; polyIndex++ )
                {
                    const int     index0    = hull.mIndices[3 * polyIndex + 0];
                    const int     index1    = hull.mIndices[3 * polyIndex + 1];
                    const int     index2    = hull.mIndices[3 * polyIndex + 2];
                    const Vector3 vec0      = hull.mVertices[index0];
                    const Vector3 vec1      = hull.mVertices[index1];
                    const Vector3 vec2      = hull.mVertices[index2];
                    const Vector3 worldVec0 = Vector3( modelMat * glm::vec4( vec0.ToGlm(), 1.f ) );
                    const Vector3 worldVec1 = Vector3( modelMat * glm::vec4( vec1.ToGlm(), 1.f ) );
                    const Vector3 worldVec2 = Vector3( modelMat * glm::vec4( vec2.ToGlm(), 1.f ) );

                    _world.GetSingleton<RenderDebug>().DebugLine( worldVec0, worldVec1, color );
                    _world.GetSingleton<RenderDebug>().DebugLine( worldVec1, worldVec2, color );
                    _world.GetSingleton<RenderDebug>().DebugLine( worldVec2, worldVec0, color );
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw all the point lights gizmos
    //==================================================================================================================================================================================================
    struct SDrawDebugPointLights : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<PointLight>() | _world.GetSignature<FxTransform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto lightIt     = _view.begin<PointLight>();
            auto transformIt = _view.begin<FxTransform>();
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
            for( ; lightIt != _view.end<PointLight>(); ++lightIt, ++transformIt )
            {
                const PointLight & light     = *lightIt;
                const FxTransform& transform = *transformIt;
                DrawPointLight( renderDebug, transform, light );
            }
        }

        static void DrawPointLight( RenderDebug& _renderDebug, const FxTransform& _transform, const PointLight& _light )
        {
            const Fixed lightRange = PointLight::GetLightRange( _light );
            if( lightRange > 0 )
            {
                _renderDebug.DebugSphere( _transform.mPosition, lightRange, _light.mDiffuse );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw all the directional lights gizmos
    //==================================================================================================================================================================================================
    struct SDrawDebugDirectionalLights : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<DirectionalLight>() | _world.GetSignature<FxTransform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto lightIt     = _view.begin<DirectionalLight>();
            auto transformIt = _view.begin<FxTransform>();
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();
            for( ; lightIt != _view.end<DirectionalLight>(); ++lightIt, ++transformIt )
            {
                const DirectionalLight& light     = *lightIt;
                const FxTransform     & transform = *transformIt;
                DrawDirectionalLight( renderDebug, transform, light );
            }
        }

        static void DrawDirectionalLight( RenderDebug& _renderDebug, const FxTransform& _transform, const DirectionalLight& /*_light*/ )
        {
            const Vector3 pos         = _transform.mPosition;
            const Vector3 dir         = _transform.Forward();
            const Vector3 up          = _transform.Up();
            const Vector3 left        = _transform.Left();
            const Fixed   length      = 2;
            const Fixed   radius      = FIXED( 0.5 );
            const Color   color       = Color::sYellow;
            Vector3       offsets[5]  = { Vector3::sZero, radius * up, -radius * up, radius * left, -radius * left };
            for( int      offsetIndex = 0; offsetIndex < 5; offsetIndex++ )
            {
                const Vector3 offset = offsets[offsetIndex];
                _renderDebug.DebugLine( pos + offset, pos + offset + length * dir, color, false );
            }
            _renderDebug.DebugIcoSphere( FxTransform::Make( _transform.mRotation, _transform.mPosition ), radius, 0, color, false );
        }
    };

    //==================================================================================================================================================================================================
    // Draw physics sphere collider in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugFxSphereColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() | _world.GetSignature<FxSphereCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<FxTransform>();
            auto sphereIt    = _view.begin<FxSphereCollider>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++sphereIt )
            {
                Draw( *sphereIt, *transformIt, renderDebug );
            }
        }

        static void Draw( const FxSphereCollider& _sphere, const FxTransform& _transform, RenderDebug& _renderDebug )
        {
            _renderDebug.DebugSphere( _transform.mPosition + _transform.TransformDirection( _sphere.mOffset ), _sphere.mRadius, Color::sGreen, false );
        }
    };

    //==================================================================================================================================================================================================
    // Draw physics box collider in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugFxBoxColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<FxTransform>() | _world.GetSignature<FxBoxCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<FxTransform>();
            auto boxIt       = _view.begin<FxBoxCollider>();
            for( ; transformIt != _view.end<FxTransform>(); ++transformIt, ++boxIt )
            {
                Draw( *boxIt, *transformIt, renderDebug );
            }
        }

        static void Draw( const FxBoxCollider& _box, const FxTransform& _transform, RenderDebug& _renderDebug )
        {
            FxTransform transform = FxTransform::Make( _transform.mRotation, _transform.mPosition );
            _renderDebug.DebugCube( transform, _box.mHalfExtents, Color::sGreen, false );
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SDrawDebugUiBounds : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() | _world.GetSignature<TagUIVisible>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& renderDebug = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<UITransform>();
            for( ; transformIt != _view.end<UITransform>(); ++transformIt )
            {
                UITransform transform = *transformIt;
                const glm::ivec2& pos  = transform.mPosition;
                const glm::ivec2& size = transform.mSize;
                renderDebug.DebugQuad2D( pos, size, Color::sGreen );
            }
        }
    };
}