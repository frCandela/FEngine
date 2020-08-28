#include "ecs/fanEcsSystem.hpp"
#include "scene/fanSceneTags.hpp"
#include "scene/components/fanBounds.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//========================================================================================================
	// Uses the rigidbody bounds to set the entity bounds
	//========================================================================================================
	struct SUpdateBoundsFromRigidbody : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<SceneNode>() |
				_world.GetSignature<Transform>() |
				_world.GetSignature<MotionState>() |
				_world.GetSignature<Rigidbody>() |
				_world.GetSignature<Bounds>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			auto rigidbodyIt = _view.begin<Rigidbody>();
			auto boundsIt = _view.begin<Bounds>();
			auto sceneNodeIt = _view.begin<SceneNode>();
			for( ; rigidbodyIt != _view.end<Rigidbody>(); ++rigidbodyIt, ++boundsIt, ++sceneNodeIt )
			{
				SceneNode& sceneNode = *sceneNodeIt;
				if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
				{
					continue;
				}

				const Rigidbody& rb = *rigidbodyIt;
				Bounds& bounds = *boundsIt;

				if( !rb.mRigidbody->isInWorld() ) { continue; }

				// gets bounds from rigidbody
				btVector3 low, high;
				rb.mRigidbody->getAabb( low, high );
				bounds.mAabb = AABB( low, high );

				sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
			}
		}
	};

	//========================================================================================================
	// Uses the convex hull in the mesh renderer mesh to generate new bounds
	//========================================================================================================
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

		static void Run( EcsWorld& /*_world*/, const EcsView& _view )
		{
			auto meshRendererIt = _view.begin<MeshRenderer>();
			auto transformIt = _view.begin<Transform>();
			auto boundsIt = _view.begin<Bounds>();
			auto sceneNodeIt = _view.begin<SceneNode>();
            for( ; meshRendererIt != _view.end<MeshRenderer>();
                   ++meshRendererIt, ++transformIt, ++boundsIt, ++sceneNodeIt )
			{
				SceneNode& sceneNode = *sceneNodeIt;
				if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
				{
					continue;
				}
				const MeshRenderer& renderer = *meshRendererIt;
				const Transform& transform = *transformIt;
				Bounds& bounds = *boundsIt;

				if( *renderer.mMesh != nullptr )
				{
					// Calculates model matrix
					const glm::vec3 position = ToGLM( transform.GetPosition() );
					const glm::vec3 scale = ToGLM( transform.mScale );
					const glm::quat rotation = ToGLM( transform.GetRotationQuat() );
					glm::mat4 modelMatrix =
					        glm::translate( glm::mat4( 1.f ), position ) *
					        glm::mat4_cast( rotation ) *
					        glm::scale( glm::mat4( 1.f ), scale );

					// Set the bounds
					bounds.mAabb = AABB( renderer.mMesh->mConvexHull.mVertices, modelMatrix );

					sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
				}
			}
		}
	};

	//========================================================================================================
	// Uses a transform bounds to set the entity bounds
	//========================================================================================================
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
			auto boundsIt = _view.begin<Bounds>();
			auto sceneNodeIt = _view.begin<SceneNode>();
			for( ; transformIt != _view.end<Transform>(); ++transformIt, ++boundsIt, ++sceneNodeIt )
			{
				SceneNode& sceneNode = *sceneNodeIt;
				if( !sceneNode.HasFlag( SceneNode::BoundsOutdated ) )
				{
					continue;
				}

				const Transform& transform = *transformIt;
				Bounds& bounds = *boundsIt;

				const btVector3 origin = transform.GetPosition();
				const float sizeBounds = 0.2f;
                bounds.mAabb = AABB( origin - sizeBounds * btVector3::One(),
                                     origin + sizeBounds * btVector3::One() );
				sceneNode.RemoveFlag( SceneNode::BoundsOutdated );
			}
		}
	};
}