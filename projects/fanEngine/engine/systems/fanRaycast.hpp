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
    struct SRaycast : EcsSystem
    {
        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Bounds>() |
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
                const SceneNode& sceneNode = *sceneNodeIt;
                if( sceneNode.HasFlag( SceneNode::NoRaycast ) ) // check NO_RAYCAST flag
                {
                    continue;
                }

                // raycast on bounds
                const Bounds& bounds = *boundsIt;
                Vector3 intersection;
                if( bounds.mAabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
                {
                    // raycast on mesh
                    const EcsEntity entity = boundsIt.GetEntity();
                    if( _world.HasComponent<MeshRenderer>( entity ) )
                    {
                        const Transform transform = *TransformIt;
                        const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
                        const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
                        if( meshRenderer.mMesh != nullptr && meshRenderer.mMesh->mConvexHull.RayCast( transformedRay.origin, transformedRay.direction, intersection ) )
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

    namespace impl
    {
        template< class FirstType, class... NextTypes >
        struct AccumulateSignature;

        template< class FirstType, class... NextTypes >
        struct AccumulateSignature
        {
            static EcsSignature Get( EcsWorld& _world ){ return _world.GetSignature<FirstType>() | AccumulateSignature<NextTypes...>::Get( _world ); }
        };

        template< class LastType>
        struct AccumulateSignature<LastType>
        {
            static EcsSignature Get( EcsWorld& _world ){ return _world.GetSignature<LastType>(); }
        };
    }

    //return _world.GetSignature<FirstType>() | AccumulateSignature<NextTypes...>( _world );

    //==================================================================================================================================================================================================
    // helper function for raycasting
    //==================================================================================================================================================================================================
    template< typename... _IncludeTypes >
    static void Raycast( EcsWorld& _world, const Ray& _ray, std::vector<EcsEntity>& _outResults )
    {
        const EcsSignature signature = SRaycast::GetSignature( _world ) | impl::AccumulateSignature<_IncludeTypes...>::Get( _world );
        EcsView            view      = _world.Match( signature, EcsSignature( 0 ) );
        if( !view.Empty() )
        {
            SRaycast::Run( _world, view, _ray, _outResults );
        }
    }
}