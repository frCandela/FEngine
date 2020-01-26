#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	class RenderGlobal {
	public:
		// editor
		static const std::set<std::string>	s_meshExtensions;
		static const std::set<std::string>	s_imagesExtensions;
		static const std::set<std::string>	s_sceneExtensions;
		static const std::set<std::string>	s_prefabExtensions;

		static const char *	s_defaultMesh;
		static const char *	s_defaultTexture;
		static const char * s_whiteTexture;
		static const char *	s_defaultIcons;
		static const char *	s_defaultImguiFont;

		static const char *	s_meshCone;
		static const char *	s_meshCube;
		static const char *	s_meshPlane;
		static const char *	s_meshSphere;
		static const char *	s_meshAxisCheck;

		static const char * s_textureUVCheck;
		static const char * s_textureWhite;

		// rendering
		static const uint32_t s_maximumNumPointLights = 16;
		static const uint32_t s_maximumNumDirectionalLight = 4;
	};
}