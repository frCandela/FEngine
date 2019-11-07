#include "fanGlobalIncludes.h"
#include "ecs/fanECSSystems.h"

#include "renderer/fanRenderer.h"
#include "core/time/fanTime.h"
#include "core/math/shapes/fanConvexHull.h"
#include "renderer/fanMesh.h"
#include "scene/fanScene.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	btVector3 ecsParticleSystem::s_cameraPosition;
	void ecsParticleSystem::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		ComponentData< ecsPosition > & _positions,
		ComponentData< ecsRotation > & /*_rotations*/,
		ComponentData< ecsMovement > & _movements,
		ComponentData< ecsParticle > & _particles ) 
	{
		std::vector<btVector3> triangles;
		std::vector<Color> colors;
		triangles.reserve( _entitiesData.size() );
		colors.reserve( _entitiesData.size() );

		const btVector3 up = btVector3::Left();
		static const float size = 0.05f;

		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) ) {
				btVector3& position = _positions.At(key).position;
				ecsMovement& movement = _movements.At( key );
				ecsParticle& particle = _particles.At( key );

				(void)particle;

				particle.durationLeft -= _delta;
				if ( particle.durationLeft < 0 ) {
					key.Kill();
				}
				position += _delta * movement.speed;

				
				btVector3 forward = s_cameraPosition - position;
				forward.normalize();
				btVector3 left = up.cross( forward );
				left.normalize();

				btMatrix3x3 mat ( 
					left[0], up[0], forward[0],
					left[1], up[1], forward[1],
					left[2], up[2], forward[2] );			
					
				triangles.push_back( position + mat * btVector3( size, 0, 0 ) );
				triangles.push_back( position + mat * btVector3( -size, 0, 0 ) );
				triangles.push_back( position + mat * btVector3( 0, 2.f*size, 0 ) );
				colors.push_back( particle.color );
			}
		}

		Debug::Render().DebugTriangles( triangles, colors );
	}


	//================================================================================================================================
	//================================================================================================================================
	void ecsPlanetsSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		,ComponentData< ecsGameobject > & _gameobjects
		,ComponentData< ecsTranform > & _transforms
		,ComponentData< ecsPlanet > &	_planets
		,ComponentData< ecsFlags > &	_flags )
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() &&  key.MatchSignature( signature::bitset ) ) {
				btTransform& transform		= _transforms.At(key).transform;
				ecsPlanet& planet			= _planets.At(key);
				const btTransform& parentTransform = _gameobjects.At( key ).gameobject->GetParent()->GetTransform()->GetBtTransform();
				ecsFlags& flags = _flags.At( key );

				float const time = -planet.speed * Time::ElapsedSinceStartup();
				btVector3 position( std::cosf( time + planet.phase ), 0, std::sinf( time + planet.phase ) );

				transform.setOrigin( parentTransform.getOrigin() + planet.radius * position);
				flags.flags |= ecsFlags::OUTDATED_AABB;
				flags.flags |= ecsFlags::OUTDATED_TRANSFORM;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchRbSystem::SynchRbToTransSystem( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		,ComponentData< ecsTranform > & _transforms
		,ComponentData< ecsMotionState > & _motionStates
		,ComponentData< ecsRigidbody > & _rigidbodies ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				btTransform& transform =	_transforms.At(key).transform;
				ecsMotionState& motionState = _motionStates.At(key);
				ecsRigidbody& rigidbody = _rigidbodies.At(key);

				rigidbody.Get().setWorldTransform( transform );
				motionState.Get().setWorldTransform( transform );
				(void)motionState; (void)rigidbody; (void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsSynchRbSystem::SynchTransToRbSystem( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &    _transforms
		, ComponentData< ecsMotionState > & _motionStates
		, ComponentData< ecsRigidbody > &   _rigidbodies ) 
	{
		for ( int entity = 0; entity < _count; entity++ ) {
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) ) {
				btRigidBody&  rigidbody = _rigidbodies.At( key ).Get();
				if( rigidbody.getInvMass() <= 0.f ){ continue; }

				btTransform& transform		= _transforms.At(key).transform;
				ecsMotionState& motionState = _motionStates.At(key);
				

				btMotionState * ms = rigidbody.getMotionState();
				ms->getWorldTransform( transform );
				(void)motionState;(void )rigidbody;(void)transform;
			}
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromHull::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &	_transforms
		, ComponentData< ecsScaling > &		_scales
		, ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsFlags > &		_flags
		, ComponentData< ecsMesh >&			_mesh )
	{
		// Find all interesting entities
		std::vector< ecsComponentsKey * > usefullEntitiesKeys;
		usefullEntitiesKeys.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) 
		{
			ecsComponentsKey & key = _entitiesData[entity];			
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t&			flags = _flags.At(key).flags;
				const ConvexHull&	hull  = _mesh.At(key).mesh->GetHull();
				if( flags & ecsFlags::OUTDATED_AABB && ! ( flags & ecsFlags::NO_AABB_UPDATE ) && ! hull.IsEmpty() )
				{
					usefullEntitiesKeys.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Calculates model matrices
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullEntitiesKeys.size() );
		for ( int dataIndex = 0; dataIndex < usefullEntitiesKeys.size(); dataIndex++ ) {
			ecsComponentsKey & key = *usefullEntitiesKeys[dataIndex];

			btTransform& transform  = _transforms.At(key).transform;
			btVector3&	 btscale	= _scales.At(key).scale;

			const glm::vec3 position	= ToGLM( transform.getOrigin() );
			const glm::vec3 scale		= ToGLM( btscale );
			const glm::quat rotation	= ToGLM( transform.getRotation() );

			modelMatrices.push_back(
				glm::translate( glm::mat4( 1.f ), position ) * glm::mat4_cast( rotation ) * glm::scale( glm::mat4( 1.f ), scale )
			);
		}

		// Calculates the new aabb using the model matrix and the convex hull
		for ( int dataIndex = 0; dataIndex < usefullEntitiesKeys.size(); dataIndex++ ) {
			ecsComponentsKey & key = *usefullEntitiesKeys[dataIndex];
			glm::mat4&			modelMatrix = modelMatrices[dataIndex];
			const ConvexHull&	hull = _mesh.At( key ).mesh->GetHull();
			AABB&				aabb = _aabbs.At(key).aabb;
			aabb = AABB( hull.GetVertices(), modelMatrix );
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromRigidbody::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		  ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsRigidbody > &	_rigidbodies
		, ComponentData< ecsFlags >    &	_flags
	)
	{
		// Find all interesting entities
		std::vector< ecsComponentsKey * > outdatedAABBEntities;
		outdatedAABBEntities.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t& flags = _flags.At( key ).flags;
				if ( flags & ecsFlags::OUTDATED_AABB && !( flags & ecsFlags::NO_AABB_UPDATE ) )
				{
					outdatedAABBEntities.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Update aabb
		for ( int dataIndex = 0; dataIndex < outdatedAABBEntities.size(); dataIndex++ )
		{
			ecsComponentsKey & key = *outdatedAABBEntities[dataIndex];

			btRigidBody& rb = _rigidbodies.At( key ).Get();
			AABB& aabb = _aabbs.At( key ).aabb;
			btVector3 low, high;
			rb.getAabb(low, high);
			aabb = AABB( low, high );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateAABBFromTransform::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
		, ComponentData< ecsTranform > &	_transforms
		, ComponentData< ecsAABB > &		_aabbs
		, ComponentData< ecsFlags > &		_flags ) {
		// Find all interesting entities
		std::vector< ecsComponentsKey * > usefullData;
		usefullData.reserve( _entitiesData.size() );
		for ( int entity = 0; entity < _count; entity++ ) 
		{
			ecsComponentsKey & key = _entitiesData[entity];
			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				uint32_t& flags = _flags.At( key ).flags;
				if ( flags & ecsFlags::OUTDATED_AABB && !( flags & ecsFlags::NO_AABB_UPDATE ) ) {
					usefullData.push_back( &key );
					flags &= ~ecsFlags::OUTDATED_AABB;
				}
			}
		}

		// Update aabb
		std::vector< glm::mat4 > modelMatrices;
		modelMatrices.reserve( usefullData .size() );
		for (int dataIndex = 0; dataIndex < usefullData.size(); dataIndex++){
			ecsComponentsKey & key = *usefullData[dataIndex];
		
			btTransform& transform	= _transforms.At(key).transform;
			AABB& aabb				= _aabbs.At(key).aabb;
			const btVector3 origin = transform.getOrigin();
				const float size = 0.05f;
				aabb = AABB( origin - size * btVector3::One(), origin + size * btVector3::One() );							
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ecsUpdateBullet::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
		ComponentData< ecsGameobject > &	_gameobjects
		, ComponentData< ecsBullet >     &	_bullets )
	{
		for ( int entity = 0; entity < _count; entity++ )
		{
			ecsComponentsKey & key = _entitiesData[entity];

			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
			{
				ecsBullet& bullet =		 _bullets.At( key );

				bullet.durationLeft -= _delta;
				if ( bullet.durationLeft <= 0.f )
				{
 					Gameobject * gameobject = _gameobjects.At( key ).gameobject;
					gameobject->GetScene()->DeleteGameobject( gameobject );
				}
			}
		}
	}
}