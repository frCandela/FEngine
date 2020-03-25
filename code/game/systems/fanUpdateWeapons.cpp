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
					// creates the bullet ( @todo use a prefab to do this )
					//if( *weapon.bulletPrefab != nullptr )
					{
						SceneNode& node = scene.CreateSceneNode( "bullet", nullptr );
						EntityID bulletID = _world.GetEntityID( node.handle );
						//EntityID bulletID =  _world.CreateEntity();

						Bullet& bullet = _world.AddComponent<Bullet>( bulletID );
						bullet.damage = weapon.bulletDamage;
						bullet.durationLeft = weapon.bulletLifeTime;

						Transform& bulletTransform = _world.AddComponent<Transform>( bulletID );
						bulletTransform.SetPosition( transform.GetPosition() + transform.TransformDirection( weapon.originOffset ) );
						bulletTransform.SetScale( 0.2f * btVector3::One() );

						MotionState& motionState = _world.AddComponent<MotionState>( bulletID );

						SphereShape& sphereShape = _world.AddComponent<SphereShape>( bulletID );
						sphereShape.SetRadius( 0.1f );

						Rigidbody& bulletRigidbody = _world.AddComponent<Rigidbody>( bulletID );
						//rigidbody->onContactStarted.Connect( &Weapon::OnBulletContact, this );
						bulletRigidbody.SetIgnoreCollisionCheck( rigidbody, true );
						bulletRigidbody.SetVelocity( rigidbody.GetVelocity() + weapon.bulletSpeed * transform.Forward() );
						bulletRigidbody.SetCollisionShape( &sphereShape.sphereShape );
						bulletRigidbody.SetMotionState( &motionState.motionState );

						physicsWorld.dynamicsWorld->addRigidBody( &bulletRigidbody.rigidbody );

						MeshRenderer& meshRenderer = _world.AddComponent<MeshRenderer>( bulletID );
						meshRenderer.mesh = Mesh::s_resourceManager.GetMesh( RenderGlobal::s_meshSphere );

						Material& material = _world.AddComponent<Material>( bulletID );
						material.texture = Texture::	s_resourceManager.GetTexture( RenderGlobal::s_whiteTexture );
						material.color = Color::Red;
					}
				}
			}
		}
	}
}