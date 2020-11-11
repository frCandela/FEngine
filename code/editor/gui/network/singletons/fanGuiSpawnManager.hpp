#prama once

#include "network/singletons/fanSpawnManager.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SpawnManager::SetInfo( EcsSingletonInfo& _info )
    {
        _info.mIcon  = ImGui::Spawn16;
        _info.mGroup = EngineGroups::Network;
        _info.mName  = "spawn manager";
        _info.onGui  = &SpawnManager::OnGui;
    }

	//========================================================================================================
	//========================================================================================================
	void SpawnManager::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		const SpawnManager& spawnManager = static_cast<const SpawnManager&>( _component );
		ImGui::Text( "pending spawns: %d", spawnManager.spawns.size() );
	}
}