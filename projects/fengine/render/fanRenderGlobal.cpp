#include "render/fanRenderGlobal.hpp"
#include "glfw/glfw3.h"

namespace fan
{
	const std::set<std::string>	RenderGlobal::sMeshExtensions   = { "gltf" };
	const std::set<std::string>	RenderGlobal::sImagesExtensions = { "png" };
	const std::set<std::string>	RenderGlobal::sSceneExtensions  = { "scene" };
	const std::set<std::string>	RenderGlobal::sPrefabExtensions = { "prefab" };
    const std::set<std::string>	RenderGlobal::sFontsExtensions = { "ttf" };

    const char* RenderGlobal::sContentPath = "/";
    const char* RenderGlobal::sFontsPath =   "fonts/";
    const char* RenderGlobal::sModelsPath =  "models/";
    const char* RenderGlobal::sScenesPath =  "scenes/";

    const char* RenderGlobal::sEditorIcon       = "_default/editorIcon.png";
    const char* RenderGlobal::sGameIcon         = "_default/gameIcon.png";
	const char* RenderGlobal::sDefaultIcons     = "_default/texture/icons.png";
	const char* RenderGlobal::sDefaultImguiFont = "_default/imgui_font.ttf";
    const char* RenderGlobal::sDefaultGameFont  = "fonts/Vera.ttf";

	const char* RenderGlobal::sMeshCone      = "_default/mesh/cone.gltf";
	const char* RenderGlobal::sMeshCube      = "_default/mesh/cube.gltf";
	const char* RenderGlobal::sMeshPlane     = "_default/mesh/plane.gltf";
	const char* RenderGlobal::sMeshSphere    = "_default/mesh/sphere.gltf";
	const char* RenderGlobal::sMeshAxisCheck = "_default/mesh/axis_check_yup.gltf";

	const char* RenderGlobal::sMesh2DQuad	= "2Dquad";

	const char* RenderGlobal::sTextureUVCheck = "_default/texture/uv_checker.png";
	const char* RenderGlobal::sTextureWhite   = "_default/texture/white.png";

	const char* RenderGlobal::sDefaultMesh    = sMeshCube;
	const char* RenderGlobal::sDefaultTexture = "_default/texture/texture.png";
	const char* RenderGlobal::sWhiteTexture   = "_default/texture/white32.png";

	const char* RenderGlobal::sDefaultVertexShader   = "shaders/empty.vert";
	const char* RenderGlobal::sDefaultFragmentShader = "shaders/empty.frag";
	const char* RenderGlobal::sImguiVertexShader     = "shaders/imgui.vert";
	const char* RenderGlobal::sImguiFragmentShader   = "shaders/imgui.frag";

	const std::vector < const char* > RenderGlobal::sDesiredValidationLayers = {
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

	const std::vector < const char* > RenderGlobal::sDesiredInstanceExtensions =
	{
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		, VK_KHR_SURFACE_EXTENSION_NAME
	};	

	const std::vector < const char* > RenderGlobal::sDesiredDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
}