#include "game/systems/fanUpdateWeapons.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanMotionState.hpp"
#include "scene/components/fanSphereShape.hpp"
#include "scene/components/fanMeshRenderer.hpp"
#include "scene/components/fanMaterial.hpp"
#include "game/components/fanBullet.hpp"
#include "game/components/fanWeapon.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_FireWeapons::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()   |
			_world.GetSignature<PlayerInput>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Weapon>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_FireWeapons::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		PhysicsWorld& physicsWorld = _world.GetSingletonComponent<PhysicsWorld>();
		Scene& scene = _world.GetSingletonComponent<Scene>();
		CollisionManager& collisionManager = _world.GetSingletonComponent<CollisionManager>();

		for( EntityID entityID : _entities )
		{
			const Transform& transform = _world.GetComponent<Transform>( entityID );
			const Rigidbody& rigidbody = _world.GetComponent<Rigidbody>( entityID );
			PlayerInput& input = _world.GetComponent<PlayerInput>( entityID );
			Weapon& weapon = _world.GetComponent<Weapon>( entityID );

			weapon.bulletsAccumulator += _delta * weapon.bulletsPerSecond;
			if( weapon.bulletsAccumulator > 1.f ) { weapon.bulletsAccumulator = 1.f; }

			if( input.inputData.fire > 0 && weapon.bulletsAccumulator >= 1.f )
			{
				--weapon.bulletsAccumulator;
				//if( m_energy->TryRemoveEnergy( m_bulletEnergyCost ) )
				{
					// creates the bullet
					if( *weapon.bulletPrefab != nullptr )
					{
						SceneNode& node = * weapon.bulletPrefab->Instanciate( *scene.root );
						EntityID bulletID = _world.GetEntityID( node.handle );

						Transform& bulletTransform = _world.GetComponent<Transform>( bulletID );
						bulletTransform.SetPosition( transform.GetPosition() + transform.TransformDirection( weapon.originOffset ) );

						Rigidbody& bulletRigidbody = _world.GetComponent<Rigidbody>( bulletID );
						bulletRigidbody.onContactStarted.Connect( &CollisionManager::OnBulletContact, &collisionManager );
						bulletRigidbody.SetIgnoreCollisionCheck( rigidbody, true );
						bulletRigidbody.SetVelocity( rigidbody.GetVelocity() + weapon.bulletSpeed * transform.Forward() );
						bulletRigidbody.SetMotionState( & _world.GetComponent<MotionState>( bulletID ).motionState );
						bulletRigidbody.SetCollisionShape( &_world.GetComponent<SphereShape>( bulletID ).sphereShape );

						physicsWorld.AddRigidbody( bulletRigidbody, node.handle );
					}
				}
			}
		}
	}
}