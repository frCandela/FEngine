# FEngine
FEngine is my 3D game engine. It uses Vulkan for Rendering, Dear ImGui for user interface and a lot of other open libraries for various purposes.

## Features (non exhaustive list): 
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
	 * Simple FBX mesh import
	 * png textures import

## Dependencies : 	
* 3D Models IO : [FBX SDK 2019.0](https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2019-0)	
* Serialization : [nlohmann json](https://github.com/nlohmann/json)
* png images : [stb_image](https://github.com/nothings/stb)
* physics : [bullet physics](https://github.com/bulletphysics/bullet3/blob/master/LICENSE.txt)
* convex hull : [Antti Kuukka - QuickHull ](https://github.com/akuukka/quickhull)
* window & input : [glfw](https://github.com/glfw/glfw)
* shader math : [glm](https://github.com/g-truc/glm)
* spirv compilation: [glslang](https://github.com/KhronosGroup/glslang)
* UI: [Dear ImGui <3](https://github.com/ocornut/imgui)
* rendering : [VulkanSDK 1.1.106.0](https://vulkan.lunarg.com/sdk/home)
	
## Screenshots
![Editor preview](/FEngine/docs/screenshot.png)
