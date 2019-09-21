#pragma once

namespace fbxsdk { 
	class FbxManager;
	class FbxScene;
	class FbxImporter;
	class FbxVector2;
	class FbxVector4;
	class FbxMesh;
}

namespace fan {

	class Mesh;

	//================================================================================================================================
	//================================================================================================================================
	class FBXImporter
	{
	public:
		FBXImporter();
		~FBXImporter();
		bool LoadScene( const std::string _path );
		bool GetMesh( Mesh  & _mesh );

	private:
		std::string m_path;

		fbxsdk::FbxManager* m_sdkManager;
		fbxsdk::FbxImporter * m_importer;
		fbxsdk::FbxScene* m_scene;

		std::vector< fbxsdk::FbxVector4 > GetNormals( const fbxsdk::FbxMesh * _mesh );
		std::vector< fbxsdk::FbxVector2 > GetUVs(const fbxsdk::FbxMesh * _mesh);

		int m_SDKMajor;
		int m_SDKMinor;
		int m_SDKRevision;
	};

}
