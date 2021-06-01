#pragma once

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
        struct Result
        {
            RaycastResult mData;
            EcsEntity     mEntity;
            static bool Sort( Result& _result1, Result& _result2 )
            {
                return _result1.mData.mDistance < _result2.mData.mDistance;
            }
        };

        static EcsSignature GetSignature( const EcsWorld& _world )
        {
            return _world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>() | _world.GetSignature<Transform>();
        }

        static void Run( EcsWorld& _world, const EcsView& _view, const Ray& _ray, std::vector<SRaycast::Result>& _outResults )
        {
            fanAssert( _ray.direction.IsNormalized() );
            _outResults.clear();

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
                RaycastResult result;
                if( bounds.mAabb.RayCast( _ray.origin, _ray.direction, result.mPosition ) == true )
                {
                    // raycast on mesh
                    const EcsEntity entity = boundsIt.GetEntity();
                    if( _world.HasComponent<MeshRenderer>( entity ) )
                    {
                        const Transform transform = *TransformIt;
                        const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
                        const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
                        if( meshRenderer.mMesh != nullptr && meshRenderer.mMesh->mConvexHull.RayCast( transformedRay, result ) )
                        {
                            if( meshRenderer.mMesh->RayCast( transformedRay, result ) )
                            {
                                SRaycast::Result result1;
                                result1.mEntity         = entity;
                                result1.mData.mPosition = transform.TransformPoint(result.mPosition);
                                result1.mData.mNormal   = transform.TransformDirection(result.mNormal);
                                result1.mData.mDistance = result.mDistance;
                                _outResults.push_back( result1 );
                            }
                        }
                    }
                    else
                    {
                        SRaycast::Result result1;
                        result1.mEntity = entity;
                        result1.mData   = result;
                        _outResults.push_back( result1 );
                    }
                }
            }

            // sorting by distance
            std::sort( _outResults.begin(), _outResults.end(), SRaycast::Result::Sort );
        }
    };

    namespace impl
    {
        template< class FirstType, class... NextTypes >
        struct AccumulateSignature;

        template< class FirstType, class... NextTypes >
        struct AccumulateSignature
        {
            static EcsSignature Get( EcsWorld& _world ) { return _world.GetSignature<FirstType>() | AccumulateSignature<NextTypes...>::Get( _world ); }
        };

        template< class LastType >
        struct AccumulateSignature<LastType>
        {
            static EcsSignature Get( EcsWorld& _world ) { return _world.GetSignature<LastType>(); }
        };
    }

    //return _world.GetSignature<FirstType>() | AccumulateSignature<NextTypes...>( _world );

    //==================================================================================================================================================================================================
    // helper function for raycasting
    //==================================================================================================================================================================================================
    template< typename... _IncludeTypes >
    static void Raycast( EcsWorld& _world, const Ray& _ray, std::vector<SRaycast::Result>& _outResults, const EcsSignature _exclude = EcsSignature( 0 ) )
    {
        const EcsSignature includeSignature = SRaycast::GetSignature( _world ) | impl::AccumulateSignature<_IncludeTypes...>::Get( _world );
        EcsView            view             = _world.Match( includeSignature, _exclude );
        if( !view.Empty() )
        {
            SRaycast::Run( _world, view, _ray, _outResults );
        }
    }
}