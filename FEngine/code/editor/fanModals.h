#pragma once

namespace std {
	namespace experimental {
		namespace filesystem {
			static string file_name(const path& _path);
		}
	}
}

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class gui {
	public:
		static const int colorEditFlags;

		static void ShowHelpMarker(const char* _desc);
		static void ToolTip( const char* _desc );
		static bool SaveFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,std::fs::path & _currentPath,int & _extensionIndex);
		static bool LoadFileModal(const char * _popupName,const std::set<std::string>& _extensionWhiteList,	std::fs::path & _path);

	private:
		static bool FilesSelector(
			const std::set<std::string>& _extensionWhiteList,
			std::fs::path & _path
		);
	};
}
