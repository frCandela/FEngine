#include "core/ecs/fanEcsSystem.hpp"
#include "core/shapes/fanRay.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/components/fanBounds.hpp"
#include "engine/components/fanMeshRenderer.hpp"
#include "engine/physics/fanTransform.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // raycast on entity bounds then on a the convex shape of the mesh when present
    // return true if the raycast
    // output a list of EcsEntity sorted by distance to the ray origin
    //==================================================================================================================================================================================================
    struct SRaycastAll : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<Bounds>() |
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Ray& _ray, std::vector<EcsEntity>& _outResults )
        {
            // Helper class for storing the result of a raycast
            struct Result
            {
                EcsEntity entityID;
                Fixed     distance;

                static bool Sort( Result& _resultA, Result& _resultB )
                {
                    return _resultA.distance < _resultB.distance;
                }
            };

            // results
            std::vector<Result> results;
            results.reserve( _view.Size() );

            // raycast
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            auto TransformIt = _view.begin<Transform>();
            for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt, ++TransformIt )
            {
                const EcsEntity entity = boundsIt.GetEntity();
                const Bounds   & bounds    = *boundsIt;
                const SceneNode& sceneNode = *sceneNodeIt;
                const Transform transform = *TransformIt;

                // check NO_RAYCAST flag
                if( sceneNode.HasFlag( SceneNode::NoRaycast ) )
                {
                    continue;
                }

                // raycast on bounds
                Vector3 intersection;
                if( bounds.mAabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
                {
                    // raycast on mesh renderer
                    if( _world.HasComponent<MeshRenderer>( entity ) )
                    {
                        const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
                        const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ),
                                                  transform.InverseTransformDirection( _ray.direction ) );
                        if( meshRenderer.mMesh != nullptr &&
                            meshRenderer.mMesh->mConvexHull.RayCast( transformedRay.origin, transformedRay.direction, intersection ) )
                        {
                            results.push_back( { entity, Vector3::SqrDistance( intersection, _ray.origin ) } );
                        }
                    }
                    else
                    {
                        results.push_back( { entity, Vector3::SqrDistance( intersection, _ray.origin ) } );
                    }
                }
            }

            // sorting by distance
            std::sort( results.begin(), results.end(), Result::Sort );

            // generating output
            _outResults.clear();
            _outResults.reserve( results.size() );
            for( Result& result : results )
            {
                _outResults.push_back( result.entityID );
            }
        }
    };

    //==================================================================================================================================================================================================
    // raycast on entity bounds then on a the convex shape of the mesh when present
    // return true if the raycast
    // output a list of EcsEntity sorted by distance to the ray origin
    //==================================================================================================================================================================================================
    struct SFxRaycastAll : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return
                    _world.GetSignature<Bounds>() |
                    _world.GetSignature<SceneNode>() |
                    _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Ray& _ray, std::vector<EcsEntity>& _outResults )
        {
            // Helper class for storing the result of a raycast
            struct Result
            {
                EcsEntity entityID;
                Fixed     distance;

                static bool Sort( Result& _resultA, Result& _resultB )
                {
                    return _resultA.distance < _resultB.distance;
                }
            };

            // results
            std::vector<Result> results;
            results.reserve( _view.Size() );

            // raycast
            auto boundsIt    = _view.begin<Bounds>();
            auto sceneNodeIt = _view.begin<SceneNode>();
            auto TransformIt = _view.begin<Transform>();
            for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt, ++TransformIt )
            {
                const EcsEntity entity = boundsIt.GetEntity();
                const Bounds   & bounds    = *boundsIt;
                const SceneNode& sceneNode = *sceneNodeIt;
                const Transform transform = *TransformIt;

                // check NO_RAYCAST flag
                if( sceneNode.HasFlag( SceneNode::NoRaycast ) )
                {
                    continue;
                }

                // raycast on bounds
                Vector3 intersection;
                if( bounds.mAabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
                {
                    // raycast on mesh renderer
                    if( _world.HasComponent<MeshRenderer>( entity ) )
                    {
                        const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
                        const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
                        if( meshRenderer.mMesh != nullptr &&
                            meshRenderer.mMesh->mConvexHull.RayCast( transformedRay.origin,
                                                                     transformedRay.direction,
                                                                     intersection ) )
                        {
                            results.push_back( { entity, Vector3::SqrDistance( intersection, _ray.origin ) } );
                        }
                    }
                    else
                    {
                        results.push_back( { entity, Vector3::SqrDistance( intersection, _ray.origin ) } );
                    }
                }
            }

            // sorting by distance
            std::sort( results.begin(), results.end(), Result::Sort );

            // generating output
            _outResults.clear();
            _outResults.reserve( results.size() );
            for( Result& result : results )
            {
                _outResults.push_back( result.entityID );
            }
        }
    };
}