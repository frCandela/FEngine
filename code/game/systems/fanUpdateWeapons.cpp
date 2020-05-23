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
#include "game/components/fanBattery.hpp"
#include "game/components/fanPlayerInput.hpp"
#include "game/singletonComponents/fanCollisionManager.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_FireWeapons::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<Transform>()   |
			_world.GetSignature<PlayerInput>() |
			_world.GetSignature<Battery>() |
			_world.GetSignature<Rigidbody>() |
			_world.GetSignature<Weapon>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_FireWeapons::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		PhysicsWorld& physicsWorld = _world.GetSingleton<PhysicsWorld>();
		Scene& scene = _world.GetSingleton<Scene>();
		CollisionManager& collisionManager = _world.GetSingleton<CollisionManager>();

		auto transformIt = _view.begin<Transform>();
		auto rigidbodyIt = _view.begin<Rigidbody>();
		auto inputIt = _view.begin<PlayerInput>();
		auto weaponIt = _view.begin<Weapon>();
		auto batteryIt = _view.begin<Battery>();
		for( ; transformIt != _view.end<Transform>(); ++transformIt, ++rigidbodyIt, ++inputIt, ++weaponIt, ++batteryIt )
		{
			const Transform& transform = *transformIt;
			const Rigidbody& rigidbody = *rigidbodyIt;
			const PlayerInput& input = *inputIt;
			Weapon& weapon = *weaponIt;
			Battery& battery = *batteryIt;

			weapon.bulletsAccumulator += _delta * weapon.bulletsPerSecond;
			if( weapon.bulletsAccumulator > 1.f ) { weapon.bulletsAccumulator = 1.f; }

			if( input.fire > 0 && weapon.bulletsAccumulator >= 1.f && battery.currentEnergy >= weapon.bulletEnergyCost )
			{
				--weapon.bulletsAccumulator;
				battery.currentEnergy -= weapon.bulletEnergyCost;

				// creates the bullet
				if( *weapon.bulletPrefab != nullptr )
				{
					SceneNode& node = *weapon.bulletPrefab->Instanciate( scene.GetRootNode() );
					EcsEntity bulletID = _world.GetEntity( node.handle );

					Transform& bulletTransform = _world.GetComponent<Transform>( bulletID );
					bulletTransform.SetPosition( transform.GetPosition() + transform.TransformDirection( weapon.originOffset ) );

					Rigidbody& bulletRigidbody = _world.GetComponent<Rigidbody>( bulletID );
					bulletRigidbody.onContactStarted.Connect( &CollisionManager::OnBulletContact, &collisionManager );
					bulletRigidbody.SetIgnoreCollisionCheck( rigidbody, true );
					bulletRigidbody.SetVelocity( rigidbody.GetVelocity() + weapon.bulletSpeed * transform.Forward() );
					bulletRigidbody.SetMotionState( _world.GetComponent<MotionState>( bulletID ).motionState );
					bulletRigidbody.SetCollisionShape( _world.GetComponent<SphereShape>( bulletID ).sphereShape );

					physicsWorld.AddRigidbody( bulletRigidbody, node.handle );
				}				
			}
		}
	}
}