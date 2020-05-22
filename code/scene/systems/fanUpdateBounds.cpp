#include "scene/systems/fanUpdateBounds.hpp"

#include "core/math/fanMathUtils.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/fanSceneTags.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateBoundsFromRigidbody::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<MotionState>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Bounds>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromRigidbody::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }


		auto rigidbodyIt = _view.begin<Rigidbody>();
		auto boundsIt = _view.begin<Bounds>();
		for( ; rigidbodyIt != _view.end<Rigidbody>(); ++rigidbodyIt, ++boundsIt )
		{
			const EcsEntity entity = rigidbodyIt.Entity();
			const Rigidbody& rb = *rigidbodyIt;
			Bounds& bounds = *boundsIt;

			if( !rb.rigidbody->isInWorld() ) { continue;}

			// gets bounds from rigidbody
			btVector3 low, high;
			rb.rigidbody->getAabb( low, high );
			bounds.aabb = AABB( low, high );

			_world.RemoveTag<tag_boundsOutdated>( entity );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateBoundsFromModel::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<MeshRenderer>() |
			_world.GetSignature<Transform>() |
			_world.GetSignature<Bounds>() |
			_world.GetSignature<tag_boundsOutdated>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromModel::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		auto meshRendererIt = _view.begin<MeshRenderer>();
		auto transformIt = _view.begin<Transform>();
		auto boundsIt = _view.begin<Bounds>();
		for( ; meshRendererIt != _view.end<MeshRenderer>(); ++meshRendererIt, ++transformIt, ++boundsIt )
		{
			const EcsEntity entity = meshRendererIt.Entity();
			const MeshRenderer& renderer = *meshRendererIt;
			const Transform& transform = *transformIt;
			Bounds& bounds = *boundsIt;

			if( *renderer.mesh != nullptr )
			{
				const ConvexHull& hull = renderer.mesh->GetHull();

				// Calculates model matrix
				const glm::vec3 position = ToGLM( transform.GetPosition() );
				const glm::vec3 scale = ToGLM( transform.scale );
				const glm::quat rotation = ToGLM( transform.GetRotationQuat() );
				glm::mat4 modelMatrix = glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), scale );

				// Set the bounds
				bounds.aabb = AABB( hull.GetVertices(), modelMatrix );

				_world.RemoveTag<tag_boundsOutdated>( entity );
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateBoundsFromTransform::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>() |
			_world.GetSignature<Bounds>() |
			_world.GetSignature<tag_boundsOutdated>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateBoundsFromTransform::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		auto transformIt = _view.begin<Transform>();
		auto boundsIt = _view.begin<Bounds>();
		for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boundsIt )
		{
			const EcsEntity entity = transformIt.Entity();
			const Transform& transform = *transformIt;
			Bounds& bounds = *boundsIt;

			const btVector3 origin = transform.GetPosition();
			const float sizeBounds = 0.2f;
			bounds.aabb = AABB( origin - sizeBounds * btVector3::One(), origin + sizeBounds * btVector3::One() );
			_world.RemoveTag<tag_boundsOutdated>( entity );
		}
	}
}