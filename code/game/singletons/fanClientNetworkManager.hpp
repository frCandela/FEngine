#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	//========================================================================================================
	struct ClientNetworkManager : public EcsSingleton
	{
		ECS_SINGLETON( ClientNetworkManager )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );			

		EcsHandle mPersistentHandle;
	};
}
