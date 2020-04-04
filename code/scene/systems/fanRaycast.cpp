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
	Signature S_RaycastAll::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>() | _world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool S_RaycastAll::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const Ray& _ray, std::vector<EntityID>& _outResults )
	{
		// Helper class for storing the result of a raycast
		struct Result
		{
			EntityID entityID;
			float distance;

			static bool Sort( Result& _resultA, Result& _resultB )
			{ 
				return _resultA.distance < _resultB.distance;
			}			
		};

		// results
		std::vector<Result> results;
		results.reserve( _entities.size() );

		// raycast
		for( EntityID entityID : _entities )
		{
			// check NO_RAYCAST flag
			const SceneNode& sceneNode = _world.GetComponent<SceneNode>( entityID );
			if( sceneNode.HasFlag( SceneNode::NO_RAYCAST ) ){ continue; }

			// raycast on bounds
			const Bounds& bounds = _world.GetComponent<Bounds>( entityID );
			btVector3 intersection;
			if( bounds.aabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
			{
				// raycast on mesh renderer
				if( _world.HasComponent<MeshRenderer>( entityID ) )
				{
					const MeshRenderer& meshRenderer = _world.GetComponent<MeshRenderer>( entityID );
					const Transform& transform = _world.GetComponent<Transform>( entityID );
					const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
					if( meshRenderer.mesh!= nullptr && meshRenderer.mesh->GetHull().RayCast( transformedRay.origin, transformedRay.direction, intersection ) )
					{
						results.push_back( { entityID , intersection.distance2( _ray.origin ) } );
					}
				}
				else
				{
					results.push_back( { entityID , intersection.distance2( _ray.origin ) } );
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