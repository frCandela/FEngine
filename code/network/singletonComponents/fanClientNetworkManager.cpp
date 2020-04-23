#include "network/singletonComponents/fanClientNetworkManager.hpp"

#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanGame.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ClientNetworkManager );

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.init = &ClientNetworkManager::Init;
		_info.onGui = &ClientNetworkManager::OnGui;
		_info.save = &ClientNetworkManager::Save;
		_info.load = &ClientNetworkManager::Load;
		_info.name = "client network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );
		netManager.world = &_world;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Sync( uint64_t _serverFrameIndex, float _rtt )
	{
		
		Game& game = world->GetSingletonComponent<Game>();
		const uint64_t target = _serverFrameIndex + uint64_t( _rtt / game.logicDelta );
		const uint64_t diff = game.frameIndex > _serverFrameIndex ? game.frameIndex - _serverFrameIndex : _serverFrameIndex - game.frameIndex;

		if( diff > 5 )
		{
			game.frameIndex = target;
		}

		//Debug::Highlight() << "server: " << _serverFrameIndex << " target: " << target << " client: " << game.frameIndex << " diff: " <<  diff << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Save( const SingletonComponent& _component, Json& _json )
	{
		const ClientNetworkManager& netManager = static_cast<const ClientNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::Load( SingletonComponent& _component, const Json& _json )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

	}

	//================================================================================================================================
	//================================================================================================================================
	void ClientNetworkManager::OnGui( EcsWorld&, SingletonComponent& _component )
	{
		ClientNetworkManager& netManager = static_cast<ClientNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}