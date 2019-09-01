#pragma once

namespace fan {
	//================================================================================================================================
	//================================================================================================================================	
	class GlobalValues {
	public:
		static const char *					s_defaultMeshPath;
		static const char *					s_defaultTexturePath;
		static const std::set<std::string>	s_meshExtensions;
		static const std::set<std::string>	s_imagesExtensions;
		static const std::set<std::string>	s_sceneExtensions;		
	};
}