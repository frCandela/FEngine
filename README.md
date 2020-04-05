# FEngine
FEngine is a 3D game engine. It uses Vulkan for Rendering, Dear ImGui for user interface and a lot of other open libraries for various purposes.

## Some features: 
* Global
	 * Game Editor
	 * Entity/Component system
	 * Scene tree & serialization
	 * Profiling tools
	 * keyboard & gamepad input
	 * client/server network
* Rendering
	 * 3D Models
	 * ui
	 * directionnal & point lights
	 * Phong illumination
	 * Postprocessing
	 * Debug lines/shapes
	 * Shader runtime compilation
	 * glTF mesh 
	 * png textures

## Dependencies : 	
* 3D Models IO : [glTF ](https://github.com/KhronosGroup/glTF)	
* Serialization : [nlohmann json](https://github.com/nlohmann/json)
* png images : [stb_image](https://github.com/nothings/stb)
* physics : [bullet physics](https://github.com/bulletphysics/bullet3/blob/master/LICENSE.txt)
* convex hull : [Antti Kuukka - QuickHull ](https://github.com/akuukka/quickhull)
* window & input : [glfw](https://github.com/glfw/glfw)
* shader math : [glm](https://github.com/g-truc/glm)
* spirv compilation: [shaderc](https://github.com/google/shaderc)
* UI: [Dear ImGui <3](https://github.com/ocornut/imgui)
* rendering : [VulkanSDK 1.1.106.0](https://vulkan.lunarg.com/sdk/home)
* network : [SFML 2.5.1](https://www.sfml-dev.org/index.php)
	
## Screenshots
![Editor preview](/docs/screenshot2.png)
