#include "game/singletons/fanCollisionManager.hpp"

#include "core/fanDebug.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanHealth.hpp"
#include "game/components/fanDamage.hpp"
#include "game/components/fanSpaceShip.hpp"
#include "scene/singletons/fanPhysicsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::RIGIDBODY16;
		_info.group = EngineGroups::Game;
		_info.name = "collision manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::Init( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//CollisionManager& collisionManager = static_cast<CollisionManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::OnBulletContact( Rigidbody& _bulletBody, Rigidbody& /*_otherBody*/, btPersistentManifold* const& /*_manifold*/ )
	{
		EcsWorld& world =  _bulletBody.GetWorld();
		const EcsHandle bulletHandle = _bulletBody.GetHandle();
		const EcsEntity bulletID = world.GetEntity( bulletHandle );
		const Bullet& bullet = world.GetComponent< Bullet >( bulletID );
		
		world.Kill( bulletID );
		
		// create explosion
		const Transform& bulletTransform = world.GetComponent< Transform >( bulletID );
		const Scene& scene = world.GetSingleton<Scene>();
		const SceneNode& explosionNode = * bullet.explosionPrefab->Instanciate( scene.GetRootNode() );
		const EcsEntity explosionID = world.GetEntity( explosionNode.handle );
		Transform& explosionTransform = world.GetComponent< Transform >( explosionID );
		explosionTransform.SetPosition( bulletTransform.GetPosition() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::OnSpaceShipContact( Rigidbody& _spaceshipBody, Rigidbody& _otherBody, btPersistentManifold* const& /*_manifold*/ )
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
			_spaceshipBody.rigidbody->applyCentralForce( spaceship.collisionRepulsionForce * dir.normalized() );
		}

		// applies damage
		if( world.HasComponent<Health>( spaceshipID ) )
		{
			if( world.HasComponent<Damage>( otherID ) )
			{
				Health& health = world.GetComponent<Health>( spaceshipID );
				if( !health.invincible )
				{
					Damage& damage = world.GetComponent<Damage>( otherID );
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