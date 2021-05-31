#include "core/ecs/fanEcsSystem.hpp"
#include "core/math/fanMatrix4.hpp"
#include "engine/fanEngineTags.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanScale.hpp"
#include "engine/physics/fanSphereCollider.hpp"
#include "engine/physics/fanBoxCollider.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Uses the convex hull in the mesh renderer mesh to generate new bounds
    //==================================================================================================================================================================================================
    struct SUpdateBoundsFromModel : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<MeshRenderer>() |
                    _world.GetSignature<Transform>() |
                    _world.GetSignature<Bounds>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view )
        {
            auto meshRendererIt = _view.begin<MeshRenderer>();
            auto transformIt    = _view.begin<Transform>();
            auto boundsIt       = _view.begin<Bounds>();
            auto sceneNodeIt    = _view.begin<SceneNode>();
            for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt, ++boundsIt, ++sceneNodeIt )
            {
                SceneNode& sceneNode = *sceneNodeIt;
                if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
                {
                    continue;
                }
                const MeshRenderer& renderer  = *meshRendererIt;
                const Transform   & transform = *transformIt;
                Bounds            & bounds    = *boundsIt;

                Scale* scaling = _world.SafeGetComponent<Scale>( boundsIt.GetEntity() );

                if( renderer.mMesh != nullptr )
                {
                    const Vector3 scale = scaling != nullptr ? scaling->mScale : Vector3 { 1, 1, 1 };

                    Matrix4 model( transform.mRotation, transform.mPosition );
                    Matrix4 scaleMatrix = Matrix4::sIdentity;
                    scaleMatrix.e11 *= scale.x;
                    scaleMatrix.e22 *= scale.y;
                    scaleMatrix.e33 *= scale.z;

                    bounds.mAabb = AABB( renderer.mMesh->mConvexHull.mVertices, model * scaleMatrix );
                    sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
                }
            }
        }
    };

    //==================================================================================================================================================================================================
    // Uses a transform bounds to set the entity bounds
    //==================================================================================================================================================================================================
    struct SUpdateBoundsFromTransform : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<Transform>() |
                    _world.GetSignature<Bounds>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto transformIt = _view.begin<Transform>();
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boundsIt, ++sceneNodeIt )
            {
                SceneNode& sceneNode = *sceneNodeIt;
                if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
                {
                    continue;
                }

                const Transform& transform = *transformIt;
                Bounds         & bounds    = *boundsIt;

                const Fixed sizeBounds = FIXED( 0.2 );
                bounds.mAabb = AABB( transform.mPosition - sizeBounds * Vector3::sOne,
                                     transform.mPosition + sizeBounds * Vector3::sOne );
                sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Uses a fixed point sphere collider set the entity bounds
    //==================================================================================================================================================================================================
    struct SUpdateBoundsFromFxSphereColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<Transform>() |
                    _world.GetSignature<SphereCollider>() |
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<Bounds>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto transformIt = _view.begin<Transform>();
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            auto sphereIt    = _view.begin<SphereCollider>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boundsIt, ++sceneNodeIt, ++sphereIt )
            {
                SceneNode& sceneNode = *sceneNodeIt;
                if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
                {
                    continue;
                }

                const Transform& transform = *transformIt;
                Bounds         & bounds    = *boundsIt;
                SphereCollider & sphere    = *sphereIt;

                const Vector3 origin = transform.mPosition;
                bounds.mAabb = AABB( transform.mPosition - sphere.mRadius * Vector3::sOne,
                                     transform.mPosition + sphere.mRadius * Vector3::sOne );
                sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
            }
        }
    };

    //==================================================================================================================================================================================================
    // Uses a fixed point sphere collider set the entity bounds
    //==================================================================================================================================================================================================
    struct SUpdateBoundsFromFxBoxColliders : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<Transform>() |
                    _world.GetSignature<BoxCollider>() |
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<Bounds>();
        }

        static void Run( EcsWorld& /*_world*/, const EcsView& _view )
        {
            auto transformIt = _view.begin<Transform>();
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            auto boxIt       = _view.begin<BoxCollider>();
            for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boundsIt, ++sceneNodeIt, ++boxIt )
            {
                SceneNode& sceneNode = *sceneNodeIt;
                if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
                {
                    continue;
                }

                const Transform& transform = *transformIt;
                Bounds         & bounds    = *boundsIt;
                BoxCollider    & box       = *boxIt;

                std::vector<Vector3> pointCloud = {
                        Vector3( box.mHalfExtents.x, box.mHalfExtents.y, box.mHalfExtents.z ),
                        Vector3( box.mHalfExtents.x, box.mHalfExtents.y, -box.mHalfExtents.z ),
                        Vector3( box.mHalfExtents.x, -box.mHalfExtents.y, box.mHalfExtents.z ),
                        Vector3( box.mHalfExtents.x, -box.mHalfExtents.y, -box.mHalfExtents.z ),
                        Vector3( -box.mHalfExtents.x, box.mHalfExtents.y, box.mHalfExtents.z ),
                        Vector3( -box.mHalfExtents.x, box.mHalfExtents.y, -box.mHalfExtents.z ),
                        Vector3( -box.mHalfExtents.x, -box.mHalfExtents.y, box.mHalfExtents.z ),
                        Vector3( -box.mHalfExtents.x, -box.mHalfExtents.y, -box.mHalfExtents.z )
                };

                // Set the bounds
                Matrix4 model( transform.mRotation, transform.mPosition );
                bounds.mAabb = AABB( pointCloud, model );
                sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
            }
        }
    };
}