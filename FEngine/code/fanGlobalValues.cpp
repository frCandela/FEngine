#include "fanGlobalIncludes.h"
#include "fanGlobalValues.h"

namespace fan {
	const char *				GlobalValues::s_defaultMeshPath		= "content/_default/mesh.fbx";
	const char *				GlobalValues::s_defaultTexturePath	= "content/_default/texture.png";
	const std::set<std::string>	GlobalValues::s_meshExtensions		= { ".fbx" };
	const std::set<std::string>	GlobalValues::s_imagesExtensions	= { ".png" };
	const std::set<std::string>	GlobalValues::s_sceneExtensions		= { ".scene" };
}