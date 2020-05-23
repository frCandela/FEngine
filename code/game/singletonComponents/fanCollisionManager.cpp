#include "game/singletonComponents/fanCollisionManager.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanDamage.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::RIGIDBODY16;
		_info.init = &CollisionManager::Init;
		_info.name = "collision manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		CollisionManager& collisionManager = static_cast<CollisionManager&>( _component );
		const_cast<EcsWorld*>( collisionManager.world ) = &_world;
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
	{
		Rigidbody* rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
		Rigidbody* rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
		Rigidbody& bulletRb = _other == rb0 ? *rb1 : *rb0;

		PhysicsWorld& physicsWorld = world->GetSingleton<PhysicsWorld>();
		const EcsHandle bulletHandle = physicsWorld.rigidbodiesHandles[&bulletRb];
		const EcsEntity bulletID = world->GetEntity( bulletHandle );
		const Bullet& bullet = world->GetComponent< Bullet >( bulletID );
		
		world->Kill( bulletID );
		
		// create explosion
		const Transform& bulletTransform = world->GetComponent< Transform >( bulletID );
		const Scene& scene = world->GetSingleton<Scene>();
		const SceneNode& explosionNode = * bullet.explosionPrefab->Instanciate( scene.GetRootNode() );
		const EcsEntity explosionID = world->GetEntity( explosionNode.handle );
		Transform& explosionTransform = world->GetComponent< Transform >( explosionID );
		explosionTransform.SetPosition( bulletTransform.GetPosition() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::OnSpaceShipContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
	{
		Rigidbody* rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
		Rigidbody* rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
		Rigidbody& spaceshipRb = _other == rb0 ? *rb1 : *rb0;
		Rigidbody& otherRb = *_other;

		// get ids
		PhysicsWorld& physicsWorld = world->GetSingleton<PhysicsWorld>();
		const EcsHandle spaceshipHandle = physicsWorld.rigidbodiesHandles[&spaceshipRb];
		const EcsHandle otherHandle = physicsWorld.rigidbodiesHandles[&otherRb];
		const EcsEntity spaceshipID = world->GetEntity( spaceshipHandle );
		const EcsEntity otherID = world->GetEntity( otherHandle );

		// bump
		const Transform& spaceshipTransform = world->GetComponent<Transform>( spaceshipID );
		const Transform& otherTransform = world->GetComponent<Transform>( otherID );
		const btVector3 dir = spaceshipTransform.GetPosition() - otherTransform.GetPosition();
		if( !dir.fuzzyZero() )
		{
			SpaceShip& spaceship = world->GetComponent<SpaceShip>( spaceshipID );
			spaceshipRb.rigidbody->applyCentralForce( spaceship.collisionRepulsionForce * dir.normalized() );
		}

		// applies damage
		if( world->HasComponent<Health>( spaceshipID ) )
		{
			if( world->HasComponent<Damage>( otherID ) )
			{
				Health& health = world->GetComponent<Health>( spaceshipID );
				if( !health.invincible )
				{
					Damage& damage = world->GetComponent<Damage>( otherID );
					health.currentHealth -= damage.damage;
					if( health.currentHealth <= 0.f )
					{
						health.currentHealth = 0;
					}
				}
			}
		}
		else
		{
			Debug::Error() << "CollisionManager: spaceship has no health !" << Debug::Endl();
		}
	}
}