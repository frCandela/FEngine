#include "scene/ecs/fanSystem.hpp"

#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanMeshRenderer2.hpp"
#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanTransform2.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Uses the rigidbody bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromRigidbody : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform2>() |
				_world.GetSignature<MotionState>() |
				_world.GetSignature<Rigidbody2>() |
				_world.GetSignature<Bounds>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID entityID : _entities )
			{
				const Rigidbody2& rb = _world.GetComponent<Rigidbody2>( entityID );
				Bounds& bounds = _world.GetComponent<Bounds>( entityID );

				// gets bounds from rigidbody
				btVector3 low, high;
				rb.rigidbody.getAabb( low, high );
				bounds.aabb = AABB( low, high );

				_world.RemoveTag<tag_boundsOutdated>( entityID );
			}
		}
	};
		
	//==============================================================================================================================================================
	// Uses the convex hull in the mesh renderer mesh to generate new bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromModel : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<MeshRenderer2>() |
				_world.GetSignature<Transform2>() |
				_world.GetSignature<Bounds>() |
				_world.GetSignature<tag_boundsOutdated>();				
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID entityID : _entities )
			{
				const MeshRenderer2& renderer = _world.GetComponent<MeshRenderer2>( entityID );
				if( *renderer.mesh != nullptr )
				{
					const Transform2& transform = _world.GetComponent<Transform2>( entityID );
					const ConvexHull& hull = renderer.mesh->GetHull();					

					// Calculates model matrix
					const glm::vec3 position = ToGLM( transform.GetPosition() );
					const glm::vec3 scale = ToGLM( transform.scale );
					const glm::quat rotation = ToGLM( transform.GetRotationQuat() );
					glm::mat4 modelMatrix = glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), scale );
					
					// Set the bounds
					Bounds& bounds = _world.GetComponent<Bounds>( entityID );
					bounds.aabb = AABB( hull.GetVertices(), modelMatrix );	

					_world.RemoveTag<tag_boundsOutdated>( entityID );
				}
			}
		}
	};

	//==============================================================================================================================================================
	// Uses a transform bounds to set the entity bounds
	//==============================================================================================================================================================
	struct S_UpdateBoundsFromTransform : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<Transform2>() |
				_world.GetSignature<Bounds>() |
				_world.GetSignature<tag_boundsOutdated>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID entityID : _entities )
			{
				const Transform2& transform = _world.GetComponent<Transform2>( entityID );				
				const btVector3 origin = transform.GetPosition();

				const float sizeBounds = 0.2f;
				Bounds& bounds = _world.GetComponent<Bounds>( entityID );
				bounds.aabb = AABB( origin - sizeBounds * btVector3::One(), origin + sizeBounds * btVector3::One() );

				_world.RemoveTag<tag_boundsOutdated>( entityID );
			}
		}
	};
}