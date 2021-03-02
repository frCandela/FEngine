#include "fanServerNetworkManager.hpp"

#include "core/fanDebug.hpp"
#include "network/singletons/fanTime.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "network/components/fanHostGameData.hpp"
#include "network/singletons/fanServerConnection.hpp"
#include "network/singletons/fanHostManager.hpp"
#include "network/systems/fanTimeout.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::SetInfo( EcsSingletonInfo& /*_info*/ )
	{
	}

	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::Init( EcsWorld& /*_world*/, EcsSingleton& /*_component*/ )
	{
		//ServerNetworkManager& netManager = static_cast<ServerNetworkManager&>( _component );
	}

	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::Start( EcsWorld& _world )
	{
		// create the network scene root for ordering net objects
		HostManager& hostManager = _world.GetSingleton<HostManager>();
		Scene& scene = _world.GetSingleton<Scene>();
		hostManager.mNetRootNodeHandle = scene.CreateSceneNode( "net root", &scene.GetRootNode() ).mHandle;

		// bind
		ServerConnection& connection = _world.GetSingleton<ServerConnection>();	
		if( connection.mSocket.Bind( connection.mServerPort ) != sf::Socket::Done )
		{
			Debug::Error() << "bind failed on port " << connection.mServerPort << Debug::Endl();
		}
		else
		{
			Debug::Log() << "bind on port " << connection.mServerPort << Debug::Endl();
		}
	}

	//========================================================================================================
	//========================================================================================================
	void ServerNetworkManager::Stop( EcsWorld& _world )
	{
		ServerConnection& connection = _world.GetSingleton<ServerConnection>();
		Debug::Log() << "unbind from port " << connection.mSocket.GetPort() << Debug::Endl();
		connection.mSocket.Unbind();
	}
}