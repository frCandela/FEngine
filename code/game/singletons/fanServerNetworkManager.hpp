#pragma once

#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	//========================================================================================================
	struct ServerNetworkManager : public EcsSingleton
	{
		ECS_SINGLETON( ServerNetworkManager )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );		
	};
}
