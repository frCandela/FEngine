#include "render/fanRenderGlobal.hpp"
#include "glfw/glfw3.h"

namespace fan
{
	const std::set<std::string>	RenderGlobal::s_meshExtensions		= { ".gltf" };
	const std::set<std::string>	RenderGlobal::s_imagesExtensions	= { ".png" };
	const std::set<std::string>	RenderGlobal::s_sceneExtensions		= { ".scene" };
	const std::set<std::string>	RenderGlobal::s_prefabExtensions	= { ".prefab" };

	const char* RenderGlobal::s_defaultIcons		= "content/_default/texture/icons.png";
	const char* RenderGlobal::s_defaultImguiFont	= "content/_default/imgui_font.ttf";

	const char* RenderGlobal::s_meshCone		= "content/_default/mesh/cone.gltf";
	const char* RenderGlobal::s_meshCube		= "content/_default/mesh/cube.gltf";
	const char* RenderGlobal::s_meshPlane		= "content/_default/mesh/plane.gltf";
	const char* RenderGlobal::s_meshSphere		= "content/_default/mesh/sphere.gltf";
	const char* RenderGlobal::s_meshAxisCheck	= "content/_default/mesh/axis_check_yup.gltf";

	const char* RenderGlobal::s_textureUVCheck	= "content/_default/texture/uv_checker.png";
	const char* RenderGlobal::s_textureWhite	= "content/_default/texture/white.png";

	const char* RenderGlobal::s_defaultMesh		= s_meshCube;
	const char* RenderGlobal::s_defaultTexture	= "content/_default/texture/texture.png";
	const char* RenderGlobal::s_whiteTexture	= "content/_default/texture/white32.png";

	const char* RenderGlobal::s_defaultVertexShader		= "code/shaders/empty.vert";
	const char* RenderGlobal::s_defaultFragmentShader	= "code/shaders/empty.frag";
	const char* RenderGlobal::s_imguiVertexShader		= "code/shaders/imgui.vert";
	const char* RenderGlobal::s_imguiFragmentShader		= "code/shaders/imgui.frag";

	const std::vector < const char* > RenderGlobal::s_desiredValidationLayers = {
#ifndef NDEBUG	
		"VK_LAYER_LUNARG_standard_validation"
		, "VK_LAYER_LUNARG_assistant_layer"
		, "VK_LAYER_LUNARG_core_validation"
		, "VK_LAYER_KHRONOS_validation"
		, "VK_LAYER_LUNARG_monitor"
		, "VK_LAYER_LUNARG_object_tracker"
		, "VK_LAYER_LUNARG_screenshot"
		, "VK_LAYER_LUNARG_standard_validation"
		, "VK_LAYER_LUNARG_parameter_validation"
#endif
	};

	const std::vector < const char* > RenderGlobal::s_desiredInstanceExtensions =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		, VK_KHR_SURFACE_EXTENSION_NAME
	};		
}