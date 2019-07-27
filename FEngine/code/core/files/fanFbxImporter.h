#pragma once

namespace scene { class Mesh; }
namespace fbxsdk { 
	class FbxManager;
	class FbxScene;
	class FbxImporter;
}


namespace util {
	
	class FBXImporter
	{
	public:
		FBXImporter();
		~FBXImporter();
		bool LoadScene( const std::string _path );
		bool GetMesh( scene::Mesh & _mesh );

	private:
		std::string m_path;

		fbxsdk::FbxManager* m_sdkManager;
		fbxsdk::FbxImporter * m_importer;
		fbxsdk::FbxScene* m_scene;

		int m_SDKMajor;
		int m_SDKMinor;
		int m_SDKRevision;
	};

}
