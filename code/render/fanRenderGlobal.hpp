#pragma once

#include <set>
#include <cstdint>
#include <string>
#include <vector>

namespace fan
{
	//================================================================================================================================
	// global values for the renderer
	//================================================================================================================================	
	class RenderGlobal
	{
	public:
		// editor
		static const std::set<std::string>	s_meshExtensions;
		static const std::set<std::string>	s_imagesExtensions;
		static const std::set<std::string>	s_sceneExtensions;
		static const std::set<std::string>	s_prefabExtensions;

		static const char* sDefaultMesh;
		static const char* sDefaultTexture;
		static const char* sWhiteTexture;
		static const char* sDefaultIcons;
		static const char* sDefaultImguiFont;

		static const char* s_meshCone;
		static const char* s_meshCube;
		static const char* s_meshPlane;
		static const char* s_meshSphere;
		static const char* s_meshAxisCheck;

        static const char* sMesh2DQuad;

		static const char* sTextureUVCheck;
		static const char* sTextureWhite;

		static const char* s_defaultVertexShader;
		static const char* s_defaultFragmentShader;
		static const char* s_imguiVertexShader;
		static const char* s_imguiFragmentShader;

		// rendering
		static const uint32_t s_maximumNumPointLights = 16;
		static const uint32_t s_maximumNumDirectionalLight = 4;

		static const std::vector < const char* > s_desiredValidationLayers;
		static const std::vector < const char* > s_desiredInstanceExtensions;
		static const std::vector < const char* > s_desiredDeviceExtensions;
	};
}