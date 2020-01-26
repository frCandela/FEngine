#include "render/fanRenderer.hpp" 

int main(int argc, char* argv[]) 
{
	fan::Renderer renderer({ 800,600 }, {0,0});
	while ( renderer.WindowIsOpen()) {
		glfwPollEvents();
		renderer.DrawFrame();
	}
	 

	 

	 
	return 0; 
}