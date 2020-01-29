#include "render/fanRenderer.hpp" 
#include "network/fanUDPSocket.hpp"

#include "scene/fanScene.hpp"
#include "game/fanGameManager.hpp"

int main(int argc, char* argv[]) 
{

	fan::GameManager manager;
	manager.Start();
// 	fan::Renderer renderer({ 800,600 }, {0,0});
// 	while ( renderer.WindowIsOpen()) {
// 		glfwPollEvents();
// 		renderer.DrawFrame();
// 	}
// 	 
// 
// 	fan::UDPSocket socket;

	 

	//delete scene;
	return 0; 
}