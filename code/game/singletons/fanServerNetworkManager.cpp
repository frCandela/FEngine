#include "game/singletons/fanServerNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "core/time/fanTime.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletons/fanGame.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/components/fanHostConnection.hpp"
#include "network/components/fanHostReplication.hpp"
#include "network/components/fanReliabilityLayer.hpp"
#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanLinkingContext.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/systems/fanTimeout.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::NETWORK16;
		_info.onGui = &ServerNetworkManager::OnGui;
		_info.name = "server network manager";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Init( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Start( EcsWorld& _world )
	{
		// create the network scene root for ordering net objects
		HostManager& hostManager = _world.GetSingleton<HostManager>();
		Scene& scene = _world.GetSingleton<Scene>();
		hostManager.netRootNodeHandle = scene.CreateSceneNode( "net root", &scene.GetRootNode() ).handle;

		// bind
		ServerConnection& connection = _world.GetSingleton<ServerConnection>();	
		if( connection.socket.Bind( connection.serverPort ) != sf::Socket::Done )
		{
			Debug::Error() << "bind failed on port " << connection.serverPort << Debug::Endl();
		}
		else
		{
			Debug::Log() << "bind on port " << connection.serverPort << Debug::Endl();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::Stop( EcsWorld& _world )
	{
		ServerConnection& connection = _world.GetSingleton<ServerConnection>();
		Debug::Log() << "unbind from port " << connection.socket.GetPort() << Debug::Endl();
		connection.socket.Unbind();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ServerNetworkManager::OnGui( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "Stop looking at me plz" );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}