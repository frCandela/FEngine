#include "scene/systems/fanRaycast.hpp"

#include "core/math/shapes/fanRay.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_RaycastAll::GetSignature( const EcsWorld& _world )
	{
		return	
			_world.GetSignature<Bounds>() | 
			_world.GetSignature<SceneNode>() | 
			_world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool S_RaycastAll::Run( EcsWorld& _world, const EcsView& _view, const Ray& _ray, std::vector<EcsEntity>& _outResults )
	{
		// Helper class for storing the result of a raycast
		struct Result
		{
			EcsEntity entityID;
			float distance;

			static bool Sort( Result& _resultA, Result& _resultB )
			{ 
				return _resultA.distance < _resultB.distance;
			}			
		};

		// results
		std::vector<Result> results;
		results.reserve( _view.Size() );

		// raycast
		auto boundsIt = _view.begin<Bounds>();
		auto sceneNodeIt = _view.begin<SceneNode>();
		auto TransformIt = _view.begin<Transform>();
		for( ; boundsIt != _view.end<Bounds>(); ++boundsIt, ++sceneNodeIt, ++TransformIt )
		{
			const EcsEntity entity = boundsIt.Entity();
			const Bounds& bounds = *boundsIt;
			const SceneNode& sceneNode = *sceneNodeIt;
			const Transform transform = *TransformIt;

			// check NO_RAYCAST flag
			if( sceneNode.HasFlag( SceneNode::NO_RAYCAST ) ){ continue; }

			// raycast on bounds
			btVector3 intersection;
			if( bounds.aabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
			{
				// raycast on mesh renderer
				if( _world.HasComponent<MeshRenderer>( entity ) )
				{
					const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entity );
					const Transform& transform = _world.GetComponent<Transform>( entity );
					const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
					if( meshRenderer.mesh!= nullptr && meshRenderer.mesh->GetHull().RayCast( transformedRay.origin, transformedRay.direction, intersection ) )
					{
						results.push_back( { entity , intersection.distance2( _ray.origin ) } );
					}
				}
				else
				{
					results.push_back( { entity , intersection.distance2( _ray.origin ) } );
				}
			}
		}

		// sorting by distance
		std::sort( results.begin(), results.end(), Result::Sort );

		// generating output
		_outResults.clear();
		_outResults.reserve( results.size() );
		for ( Result& result : results )
		{
			_outResults.push_back( result.entityID );
		}

		return ! _outResults.empty();
	}
}