#include "ecs/fanEcsSystem.hpp"
#include <stack>
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/ui/fanUITransform.hpp"
#include "engine/ui/fanUIRenderer.hpp"
#include "engine/ui/fanUILayout.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/fanEngineTags.hpp"
#include "engine/physics/fanSphereCollider.hpp"
#include "engine/physics/fanBoxCollider.hpp"
#include "engine/components/fanSkinnedMeshRenderer.hpp"
#include "fanRaycastUI.hpp"

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
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt )
            {
                const Bounds& bounds = *boundsIt;
                rd.DrawAABB( bounds.mAabb, Color::sRed );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Draw the bounds of all scene nodes
    //==================================================================================================================================================================================================
    struct SDrawDebugBoundingSpheres : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                if( meshRenderer.mMesh == nullptr ){ continue; }

                const Transform& transform = *transformIt;
                const Sphere   & sphere    = meshRenderer.mMesh->mBoundingSphere;

                rd.DrawSphere( transform * sphere.mCenter, sphere.mRadius, Color::sOrange );
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
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                const Transform   & transform    = *transformIt;

                if( meshRenderer.mMesh != nullptr )
                {
                    const glm::mat4 modelMat  = transform.GetModelMatrix();
                    const glm::mat4 normalMat = transform.GetNormalMatrix();
                    for( SubMesh& subMesh : meshRenderer.mMesh->mSubMeshes )
                    {
                        const std::vector<uint32_t>& indices  = subMesh.mIndices;
                        const std::vector<Vertex>  & vertices = subMesh.mVertices;
                        for( int index = 0; index < (int)indices.size(); index++ )
                        {
                            const Vertex& vertex = vertices[indices[index]];
                            const Vector3 position = Vector3( modelMat * glm::vec4( vertex.mPos, 1.f ) );
                            const Vector3 normal   = Vector3( normalMat * glm::vec4( vertex.mNormal, 1.f ) );
                            rd.DrawLine( position, position + FIXED( 0.3 ) * normal, Color::sGreen, true );
                        }
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
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                const Transform   & transform    = *transformIt;
                if( meshRenderer.mMesh == nullptr ){ continue; }

                const glm::mat4 modelMat = transform.GetModelMatrix();
                for( SubMesh& subMesh : meshRenderer.mMesh->mSubMeshes )
                {
                    const std::vector<uint32_t>& indices  = subMesh.mIndices;
                    const std::vector<Vertex>  & vertices = subMesh.mVertices;

                    for( int index = 0; index < (int)indices.size() / 3; index++ )
                    {
                        Vector3 v0 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 0]].mPos, 1.f ) );
                        Vector3 v1 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 1]].mPos, 1.f ) );
                        Vector3 v2 = Vector3( modelMat * glm::vec4( vertices[indices[3 * index + 2]].mPos, 1.f ) );
                        rd.DrawLine( v0, v1, Color( 0, 1, 1, 0.6f ), true );
                        rd.DrawLine( v1, v2, Color( 0, 1, 1, 0.6f ), true );
                        rd.DrawLine( v2, v0, Color( 0, 1, 1, 0.6f ), true );
                    }
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
            return _world.GetSignature<MeshRenderer>() | _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt )
            {
                const MeshRenderer& meshRenderer = *meshRendererIt;
                if( meshRenderer.mMesh == nullptr ){ continue; }

                const Transform& transform = *transformIt;

                const ConvexHull& hull = meshRenderer.mMesh->mConvexHull;
                if( hull.mVertices.empty() ){ continue; }

                const glm::mat4 modelMat = transform.GetModelMatrix();

                Color         color     = Color( 0, .7f, 1.f, 0.5f );
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

                    rd.DrawLine( worldVec0, worldVec1, color, true );
                    rd.DrawLine( worldVec1, worldVec2, color, true );
                    rd.DrawLine( worldVec2, worldVec0, color, true );
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
            return _world.GetSignature<PointLight>() | _world.GetSignature<Transform>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto lightIt     = _view.begin<PointLight>();
            auto transformIt = _view.begin<Transform>();
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( ; lightIt != _view.end<PointLight>(); ++lightIt, ++transformIt )
            {
                const PointLight& light     = *lightIt;
                const Transform & transform = *transformIt;
                DrawPointLight( rd, transform, light );
            }
        }

        static void DrawPointLight( RenderDebug& _renderDebug, const Transform& _transform, const PointLight& _light )
        {
            const Fixed lightRange = PointLight::GetLightRange( _light );
            if( lightRange > 0 )
            {
                _renderDebug.DrawSphere( _transform.mPosition, lightRange, _light.mDiffuse );
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
            return _world.GetSignature<DirectionalLight>() | _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto lightIt     = _view.begin<DirectionalLight>();
            auto transformIt = _view.begin<Transform>();
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();
            for( ; lightIt != _view.end<DirectionalLight>(); ++lightIt, ++transformIt )
            {
                const DirectionalLight& light     = *lightIt;
                const Transform       & transform = *transformIt;
                DrawDirectionalLight( rd, transform, light );
            }
        }

        static void DrawDirectionalLight( RenderDebug& _rd, const Transform& _transform, const DirectionalLight& /*_light*/ )
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
                _rd.DrawLine( pos + offset, pos + offset + length * dir, color, false );
            }
            _rd.DrawIcoSphere( Transform::Make( _transform.mRotation, _transform.mPosition ), radius, 0, color, false );
        }
    };

    //==================================================================================================================================================================================================
    // Draw physics sphere collider in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugFxSphereColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() | _world.GetSignature<SphereCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<Transform>();
            auto sphereIt    = _view.begin<SphereCollider>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++sphereIt )
            {
                Draw( *sphereIt, *transformIt, rd );
            }
        }

        static void Draw( const SphereCollider& _sphere, const Transform& _transform, RenderDebug& _rd )
        {
            _rd.DrawSphere( _transform.mPosition + _transform.TransformDirection( _sphere.mOffset ), _sphere.mRadius, Color::sGreen, false );
        }
    };

    //==================================================================================================================================================================================================
    // Draw physics box collider in wireframe
    //==================================================================================================================================================================================================
    struct SDrawDebugFxBoxColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() | _world.GetSignature<BoxCollider>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<Transform>();
            auto boxIt       = _view.begin<BoxCollider>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boxIt )
            {
                Draw( *boxIt, *transformIt, rd );
            }
        }

        static void Draw( const BoxCollider& _box, const Transform& _transform, RenderDebug& _rd )
        {
            Transform transform = Transform::Make( _transform.mRotation, _transform.mPosition );
            _rd.DrawCube( transform, _box.mHalfExtents, Color::sGreen, false );
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SDrawDebugUiBounds : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<UITransform>() | _world.GetSignature<TagVisible>();
        }
        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<UITransform>();
            for( ; transformIt != _view.end<UITransform>(); ++transformIt )
            {
                UITransform transform = *transformIt;
                const glm::ivec2& pos  = transform.mPosition;
                const glm::ivec2& size = transform.mSize;
                rd.DrawQuad2D( pos, size, Color::sGreen );
            }
        }
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SDrawSkeletons : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Transform>() | _world.GetSignature<SkinnedMeshRenderer>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            RenderDebug& rd = _world.GetSingleton<RenderDebug>();

            auto transformIt = _view.begin<Transform>();
            auto rendererIt  = _view.begin<SkinnedMeshRenderer>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++rendererIt )
            {
                Transform           transform = *transformIt;
                SkinnedMeshRenderer renderer  = *rendererIt;

                if( renderer.mMesh != nullptr )
                {
                    struct BoneData
                    {
                        int     mIndex;
                        Matrix4 mParentTransform;
                    };
                    Skeleton& skeleton = renderer.mMesh->mSkeleton;
                    const Matrix4 modelMat = Matrix4( transform.mRotation, transform.mPosition );
                    BoneData             root = { 0, modelMat };
                    std::stack<BoneData> stack;
                    stack.push( root );

                    while( !stack.empty() )
                    {
                        BoneData boneData = stack.top();
                        stack.pop();

                        Matrix4 childTransform = modelMat * skeleton.mInverseBindMatrix[boneData.mIndex].Inverse();
                        rd.DrawPoint( childTransform.GetOrigin(), FIXED( 0.05 ), Color::sRed );
                        if( boneData.mIndex != 0 )
                        {
                            rd.DrawLine( childTransform.GetOrigin(), boneData.mParentTransform.GetOrigin(), Color::sRed );
                        }

                        Bone& bone = skeleton.mBones[boneData.mIndex];
                        for( int i = 0; i < bone.mNumChilds; i++ )
                        {
                            stack.push( { bone.mChilds[i], childTransform } );
                        }
                    }
                }
            }
        }
    };
}