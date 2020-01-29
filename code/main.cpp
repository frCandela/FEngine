#include "render/fanRenderer.hpp" 
#include "network/fanUDPSocket.hpp"

#include "scene/fanScene.hpp"

int main(int argc, char* argv[]) 
{
	fan::Scene * scene = new fan::Scene("toto");
// 	fan::Renderer renderer({ 800,600 }, {0,0});
// 	while ( renderer.WindowIsOpen()) {
// 		glfwPollEvents();
// 		renderer.DrawFrame();
// 	}
// 	 
// 
// 	fan::UDPSocket socket;

	delete scene;
	return 0; 
}