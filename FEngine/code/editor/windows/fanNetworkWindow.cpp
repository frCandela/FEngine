#include "fanGlobalIncludes.h"
#include "editor/windows/fanNetworkWindow.h"

#include "network/fanClient.h"
#include "network/fanServer.h"


namespace fan {

	//================================================================================================================================
	//================================================================================================================================
	NetworkWindow::NetworkWindow( ) :	EditorWindow("network", ImGui::IconType::JOYSTICK16 )
		, m_client1("fan", 53001 )
		, m_server(53000 )
	{

	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::OnGui() 
	{
		// CLIENT
		if ( ImGui::Button( "client bind" ) )
		{
			m_client1.Bind();
		}

		if ( ImGui::Button( "client connect to server" ) )
		{
			m_client1.ConnectToServer(53000, "127.0.0.1");
		}	

		// SERVER
		if ( ImGui::Button( "server bind" ) )	
		{
			m_server.Bind();
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void NetworkWindow::Update( const float _delta )
	{
		m_client1.Update(_delta);	
		m_server.Update(_delta);	
	}
}