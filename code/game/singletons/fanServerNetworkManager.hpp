#pragma once

#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
	class EcsWorld;

	//========================================================================================================
	//========================================================================================================
	struct ServerNetworkManager : public EcsSingleton
	{
		ECS_SINGLETON( ServerNetworkManager )
	public:
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void OnGui( EcsWorld&, EcsSingleton& _component );

		void Start( EcsWorld& _world );
		void Stop( EcsWorld& _world );		
	};
}
