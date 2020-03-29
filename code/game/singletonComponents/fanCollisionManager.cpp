#include "game/singletonComponents/fanCollisionManager.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "scene/components/fanRigidbody.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "game/components/fanBullet.hpp"
#include "scene/singletonComponents/fanPhysicsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( CollisionManager );

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::RIGIDBODY16;
		_info.init = &CollisionManager::Init;
		_info.name = "collision manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::Init( SingletonComponent& _component )
	{
		CollisionManager& collisionManager = static_cast<CollisionManager&>( _component );
		collisionManager.world = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void CollisionManager::OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
	{
		assert( world != nullptr );
		Rigidbody* rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
		Rigidbody* rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
		Rigidbody& bulletRb = _other == rb0 ? *rb1 : *rb0;

		PhysicsWorld& physicsWorld = world->GetSingletonComponent<PhysicsWorld>();
		EntityHandle bulletHandle = physicsWorld.rigidbodiesHandles[&bulletRb];
		EntityID bulletID = world->GetEntityID( bulletHandle );
		world->KillEntity( bulletID );

		Scene& scene = world->GetSingletonComponent<Scene>();
		Bullet& bullet = world->GetComponent< Bullet >( bulletID );
		Transform& bulletTransform = world->GetComponent< Transform >( bulletID );
		

		SceneNode& explosionNode = * bullet.explosionPrefab->Instanciate( *scene.root );
		EntityID explosionID = world->GetEntityID( explosionNode.handle );
		Transform& explosionTransform = world->GetComponent< Transform >( explosionID );
		explosionTransform.SetPosition( bulletTransform.GetPosition() );

		//Bullet& bullet

// 		m_gameobject->GetScene().DeleteGameobject( &bulletRb->GetGameobject() );
// 		if( GetScene().IsServer() == false )
// 		{
// 			CreateExplosion( bulletRb->GetGameobject().GetTransform().GetPosition() );
// 		}
	}
}