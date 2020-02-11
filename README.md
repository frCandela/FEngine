# FEngine
FEngine is a 3D game engine. It uses Vulkan for Rendering, Dear ImGui for user interface and a lot of other open libraries for various purposes.

## Features: 
* Scene management
	 * Scene Editor
	 * Entity/Component system
	 * Save/Load
	 * Various component ( transform, mesh, material, actors etc.)
* Rendering with vulkan
	 * Mesh display
	 * Simple phong illumination
	 * Postprocessing
	 * Debug lines/shapes
	 * Shader runtime compilation
* Other
	 * Simple glTF mesh import
	 * png textures import

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
* bitsets : [ClaasBontus - bitset2](https://github.com/ClaasBontus/bitset2)
* network : [SFML 2.5.1](https://www.sfml-dev.org/index.php)
	
## Screenshots
![Editor preview](/FEngine/docs/screenshot.png)
