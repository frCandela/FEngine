#include "scene/ecs/systems/fanRaycast.hpp"

#include "core/math/shapes/fanRay.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanTransform2.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_RaycastAll::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>() | _world.GetSignature<Transform2>();
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
			Bounds& bounds = _world.GetComponent<Bounds>( entityID );

			// raycast on bounds
			btVector3 intersection;
			if( bounds.aabb.RayCast( _ray.origin, _ray.direction, intersection ) == true )
			{
				// raycast on mesh renderer
				if( _world.HasComponent<MeshRenderer2>( entityID ) )
				{
					MeshRenderer2& meshRenderer = _world.GetComponent<MeshRenderer2>( entityID );
					Transform2& transform = _world.GetComponent<Transform2>( entityID );
					const Ray transformedRay( transform.InverseTransformPoint( _ray.origin ), transform.InverseTransformDirection( _ray.direction ) );
					if( meshRenderer.mesh->GetHull().RayCast( transformedRay.origin, transformedRay.direction, intersection ) )
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