#include "fanGlobalIncludes.h"
#include "fanGlobalValues.h"

namespace fan {
	const std::set<std::string>	GlobalValues::s_meshExtensions   = { ".fbx" };
	const std::set<std::string>	GlobalValues::s_imagesExtensions = { ".png" };
	const std::set<std::string>	GlobalValues::s_sceneExtensions  = { ".scene" };
	const std::set<std::string>	GlobalValues::s_prefabExtensions  = { ".prefab" };
	

	const char *	GlobalValues::s_defaultMesh			= "content/_default/mesh/mesh.fbx";
	const char *	GlobalValues::s_defaultTexture		= "content/_default/texture/texture.png";
	const char *	GlobalValues::s_defaultIcons		= "content/_default/texture/icons.png";
	const char *	GlobalValues::s_defaultImguiFont	= "content/_default/imgui_font.ttf";
	
	const char *	GlobalValues::s_meshCone		= "content/_default/mesh/cone.fbx";
	const char *	GlobalValues::s_meshCube		= "content/_default/mesh/cube.fbx";
	const char *	GlobalValues::s_meshPlane		= "content/_default/mesh/plane.fbx";
	const char *	GlobalValues::s_meshSphere		= "content/_default/mesh/sphere.fbx";
	const char *	GlobalValues::s_meshAxisCheck	= "content/_default/mesh/axis_check.fbx";

	const char *	GlobalValues::s_textureUVCheck	= "content/_default/texture/uv_checker.png";
	const char *	GlobalValues::s_textureWhite	= "content/_default/texture/white.png";

}