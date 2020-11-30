#include "project_spaceships/singletons/fanCollisionManager.hpp"

#include "core/fanDebug.hpp"
#include "engine/components/fanRigidbody.hpp"
#include "engine/components/fanTransform.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/singletons/fanScene.hpp"
#include "project_spaceships/components/fanBullet.hpp"
#include "project_spaceships/components/fanHealth.hpp"
#include "project_spaceships/components/fanDamage.hpp"
#include "project_spaceships/components/fanSpaceShip.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void CollisionManager::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void CollisionManager::Init( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//CollisionManager& collisionManager = static_cast<CollisionManager&>( _component );

	}

	//========================================================================================================
	//========================================================================================================
    void CollisionManager::OnBulletContact( Rigidbody& _bulletBody,
                                            Rigidbody& /*_otherBody*/,
                                            btPersistentManifold* const& /*_manifold*/ )
	{
		EcsWorld& world =  _bulletBody.GetWorld();
		const EcsHandle bulletHandle = _bulletBody.GetHandle();
		const EcsEntity bulletID = world.GetEntity( bulletHandle );
		const Bullet& bullet = world.GetComponent< Bullet >( bulletID );
		
		world.Kill( bulletID );
		
		// create explosion
		const Transform& bulletTransform = world.GetComponent< Transform >( bulletID );
		const Scene& scene = world.GetSingleton<Scene>();
		const SceneNode& explosionNode = *bullet.mExplosionPrefab->Instantiate( scene.GetRootNode() );
		const EcsEntity explosionID = world.GetEntity( explosionNode.mHandle );
		Transform& explosionTransform = world.GetComponent< Transform >( explosionID );
		explosionTransform.SetPosition( bulletTransform.GetPosition() );
	}

	//========================================================================================================
	//========================================================================================================
    void CollisionManager::OnSpaceShipContact( Rigidbody& _spaceshipBody,
                                               Rigidbody& _otherBody,
                                               btPersistentManifold* const& /*_manifold*/ )
	{
		// get ids
		EcsWorld& world = _spaceshipBody.GetWorld();
		const EcsHandle spaceshipHandle = _spaceshipBody.GetHandle();
		const EcsHandle otherHandle = _otherBody.GetHandle();
		const EcsEntity spaceshipID = world.GetEntity( spaceshipHandle );
		const EcsEntity otherID = world.GetEntity( otherHandle );

		// bump
		const Transform& spaceshipTransform = world.GetComponent<Transform>( spaceshipID );
		const Transform& otherTransform = world.GetComponent<Transform>( otherID );
		const btVector3 dir = spaceshipTransform.GetPosition() - otherTransform.GetPosition();
		if( !dir.fuzzyZero() )
		{
			SpaceShip& spaceship = world.GetComponent<SpaceShip>( spaceshipID );
            _spaceshipBody.mRigidbody->applyCentralForce( spaceship.mCollisionRepulsionForce *
                                                          dir.normalized() );
		}

		// applies damage
		if( world.HasComponent<Health>( spaceshipID ) )
		{
			if( world.HasComponent<Damage>( otherID ) )
			{
				Health& health = world.GetComponent<Health>( spaceshipID );
				if( !health.mInvincible )
				{
					Damage& damage = world.GetComponent<Damage>( otherID );
					health.mCurrentHealth -= damage.mDamage;
					if( health.mCurrentHealth <= 0.f )
					{
						health.mCurrentHealth = 0;
					}
				}
			}
		}
		else
		{
			Debug::Error( "CollisionManager: spaceship has no health !" );
		}
	}
}