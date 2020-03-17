#include "scene/ecs/systems/fanUpdateBounds.hpp"

#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanMeshRenderer.hpp"
#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanTransform.hpp"
#include "scene/ecs/components/fanMotionState.hpp"
#include "scene/ecs/components/fanRigidbody.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateBoundsFromRigidbody::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<MotionState>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Bounds>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromRigidbody::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			const Rigidbody& rb = _world.GetComponent<Rigidbody>( entityID );
			Bounds& bounds = _world.GetComponent<Bounds>( entityID );

			// gets bounds from rigidbody
			btVector3 low, high;
			rb.rigidbody.getAabb( low, high );
			bounds.aabb = AABB( low, high );

			_world.RemoveTag<tag_boundsOutdated>( entityID );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateBoundsFromModel::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<MeshRenderer>() |
			_world.GetSignature<Transform>() |
			_world.GetSignature<Bounds>() |
			_world.GetSignature<tag_boundsOutdated>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromModel::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			const MeshRenderer& renderer = _world.GetComponent<MeshRenderer>( entityID );
			if( *renderer.mesh != nullptr )
			{
				const Transform& transform = _world.GetComponent<Transform>( entityID );
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

	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateBoundsFromTransform::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<Bounds>() |
			_world.GetSignature<tag_boundsOutdated>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromTransform::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			const Transform& transform = _world.GetComponent<Transform>( entityID );
			const btVector3 origin = transform.GetPosition();

			const float sizeBounds = 0.2f;
			Bounds& bounds = _world.GetComponent<Bounds>( entityID );
			bounds.aabb = AABB( origin - sizeBounds * btVector3::One(), origin + sizeBounds * btVector3::One() );

			_world.RemoveTag<tag_boundsOutdated>( entityID );
		}
	}
}