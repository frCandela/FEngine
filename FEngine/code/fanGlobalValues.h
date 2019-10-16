#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	class GlobalValues {
	public:
		// editor
		static const char *					s_defaultMeshPath;
		static const char *					s_defaultTexturePath;
		static const std::set<std::string>	s_meshExtensions;
		static const std::set<std::string>	s_imagesExtensions;
		static const std::set<std::string>	s_sceneExtensions;	

		// rendering
		static const uint32_t s_maximumNumPointLights = 16;
		static const uint32_t s_maximumNumDirectionalLight = 4;
	};
}