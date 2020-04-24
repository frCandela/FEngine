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
	void ClientNetworkManager::ShiftFrameIndex( const int64_t _framesDelta  )
	{
		Game& game = world->GetSingletonComponent<Game>();		
		game.frameIndex += _framesDelta;

		Debug::Warning() << "Shifted client frame index : " << _framesDelta << Debug::Endl();
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