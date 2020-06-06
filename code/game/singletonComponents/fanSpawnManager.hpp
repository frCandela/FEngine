#pragma once

#include "ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"
#include "bullet/LinearMath/btVector3.h"
#include "network/components/fanClientRPC.hpp"

namespace fan
{
	struct Rigidbody;
	class EcsWorld;

	//================================================================================================================================
	//================================================================================================================================	
	struct SpawnManager : public EcsSingleton
	{
		ECS_SINGLETON( SpawnManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		std::vector<ClientRPC::BulletSpawnInfo> bullets;

		static void Spawn( EcsWorld& _world );
		void OnSpawnBullet( ClientRPC::BulletSpawnInfo _info ) { bullets.push_back( _info ); }
	};
}