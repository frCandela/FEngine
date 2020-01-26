#include "render/fanRenderGlobal.hpp"

namespace fan {
	const std::set<std::string>	RenderGlobal::s_meshExtensions   = { ".fbx" };
	const std::set<std::string>	RenderGlobal::s_imagesExtensions = { ".png" };
	const std::set<std::string>	RenderGlobal::s_sceneExtensions  = { ".scene" };
	const std::set<std::string>	RenderGlobal::s_prefabExtensions  = { ".prefab" };
	

	const char *	RenderGlobal::s_defaultMesh			= "content/_default/mesh/mesh.fbx";
	const char *	RenderGlobal::s_defaultTexture		= "content/_default/texture/texture.png";
	const char *	RenderGlobal::s_whiteTexture		= "content/_default/texture/white32.png";
	
	const char *	RenderGlobal::s_defaultIcons		= "content/_default/texture/icons.png";
	const char *	RenderGlobal::s_defaultImguiFont	= "content/_default/imgui_font.ttf";
	
	const char *	RenderGlobal::s_meshCone		= "content/_default/mesh/cone.fbx";
	const char *	RenderGlobal::s_meshCube		= "content/_default/mesh/cube.fbx";
	const char *	RenderGlobal::s_meshPlane		= "content/_default/mesh/plane.fbx";
	const char *	RenderGlobal::s_meshSphere		= "content/_default/mesh/sphere.fbx";
	const char *	RenderGlobal::s_meshAxisCheck	= "content/_default/mesh/axis_check.fbx";

	const char *	RenderGlobal::s_textureUVCheck	= "content/_default/texture/uv_checker.png";
	const char *	RenderGlobal::s_textureWhite	= "content/_default/texture/white.png";

}