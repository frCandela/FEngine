#include "render/fanRenderGlobal.hpp"

namespace fan {
	const std::set<std::string>	RenderGlobal::s_meshExtensions   = { ".gltf" };
	const std::set<std::string>	RenderGlobal::s_imagesExtensions = { ".png" };
	const std::set<std::string>	RenderGlobal::s_sceneExtensions  = { ".scene" };
	const std::set<std::string>	RenderGlobal::s_prefabExtensions  = { ".prefab" };
	
	const char *	RenderGlobal::s_defaultIcons		= "content/_default/texture/icons.png";
	const char *	RenderGlobal::s_defaultImguiFont	= "content/_default/imgui_font.ttf";
	
	const char *	RenderGlobal::s_meshCone		= "content/_default/mesh/cone.gltf";
	const char *	RenderGlobal::s_meshCube		= "content/_default/mesh/cube.gltf";
	const char *	RenderGlobal::s_meshPlane		= "content/_default/mesh/plane.gltf";
	const char *	RenderGlobal::s_meshSphere		= "content/_default/mesh/sphere.gltf";
	const char *	RenderGlobal::s_meshAxisCheck	= "content/_default/mesh/axis_check_yup.gltf";

	const char *	RenderGlobal::s_textureUVCheck	= "content/_default/texture/uv_checker.png";
	const char *	RenderGlobal::s_textureWhite	= "content/_default/texture/white.png";

	const char* RenderGlobal::s_defaultMesh = s_meshCube;
	const char* RenderGlobal::s_defaultTexture = "content/_default/texture/texture.png";
	const char* RenderGlobal::s_whiteTexture = "content/_default/texture/white32.png";

}