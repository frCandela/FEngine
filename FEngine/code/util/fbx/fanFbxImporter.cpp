#include "fanIncludes.h"

#include "util/fbx/fanFbxImporter.h"
#include "scene/components/fanMesh.h"
#include "scene/fanGameobject.h"
#include "fbxsdk.h"


namespace util {
	//================================================================================================================================
	//================================================================================================================================
	FBXImporter::FBXImporter() {

		//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
		m_sdkManager = FbxManager::Create();
		if (m_sdkManager == nullptr ) {
			std::cout << "Error: Unable to create FBX Manager!\n" << std::endl;
		}
		else {
			std::cout << "Autodesk FBX SDK version" << m_sdkManager->GetVersion() << std::endl;
		}

		// fbx manager
		FbxIOSettings* ios = FbxIOSettings::Create(m_sdkManager, IOSROOT);
		m_sdkManager->SetIOSettings(ios);
		FbxString path = FbxGetApplicationDirectory();
		m_sdkManager->LoadPluginsDirectory(path.Buffer());

		// fbx scene
		m_scene = FbxScene::Create(m_sdkManager, "fbx scene");
		if (m_scene == nullptr ) {
			std::cout << "Error: Unable to create FBX scene!\n" << std::endl;
		}

		FbxManager::GetFileFormatVersion(m_SDKMajor, m_SDKMinor, m_SDKRevision);
		m_importer = FbxImporter::Create(m_sdkManager, "");
	}

	//================================================================================================================================
	//================================================================================================================================
	FBXImporter::~FBXImporter() {
		m_importer->Destroy();
		m_scene->Destroy();
		m_sdkManager->Destroy();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FBXImporter::LoadScene(const std::string _path) {
		m_path = _path;

		int fileMajor, fileMinor, fileRevision;

		const bool importStatus = m_importer->Initialize(m_path.c_str(), -1, m_sdkManager->GetIOSettings());
		m_importer->GetFileVersion(fileMajor, fileMinor, fileRevision);

		if ( importStatus == false )
		{
			fbxsdk::FbxString error = m_importer->GetStatus().GetErrorString();
			std::cout << "Call to FbxImporter::Initialize() failed." << std::endl;
			std::cout << "Error returned: " <<  error.Buffer()  << " " << m_path << std::endl;

			if (m_importer->GetStatus().GetCode() == fbxsdk::FbxStatus::eInvalidFileVersion)
			{
				std::cout << "FBX file format version for this FBX SDK is " << m_SDKMajor << "." << m_SDKMinor << "." << m_SDKRevision << std::endl;
				std::cout << "FBX file format version for file " << m_path << " is " << fileMajor << "." << fileMinor << "." << fileRevision << std::endl;
			}
			return false;
		}

		if (m_importer->IsFBX() == false) {
			std::cout << "Error: File is not an fbx" << std::endl;
			return false;
		}

		bool status = m_importer->Import( m_scene );
		if (status == false ) {
			if (m_importer->GetStatus().GetCode() == fbxsdk::FbxStatus::ePasswordError) {
				std::cout << "password is wrong" << std::endl;
			}
			std::cout << "import failed" << std::endl;
			return false;
		}

		std::cout << "successfully imported " << m_path << std::endl;		

		fbxsdk::FbxGeometryConverter geometryConverter( m_sdkManager );
		geometryConverter.Triangulate(m_scene, true);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FBXImporter::GetMesh(scene::Mesh & _mesh) {

		std::queue< fbxsdk::FbxNode*> nodesQueue;
		nodesQueue.push(m_scene->GetRootNode());

		// Find a mesh in the nodes hierarchy
		fbxsdk::FbxMesh * mesh = nullptr;
		while (nodesQueue.empty() == false ) {
			fbxsdk::FbxNode * node = nodesQueue.front();
			nodesQueue.pop();

			for (int childIndex = 0; childIndex < node->GetChildCount(); childIndex++) {
				nodesQueue.push(node->GetChild(childIndex));
			}

			fbxsdk::FbxNodeAttribute * attribute = node->GetNodeAttribute();
			if (attribute != nullptr) {
				if (attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eMesh) {
					mesh  = static_cast<fbxsdk::FbxMesh*>(node->GetNodeAttribute());
					break;
				}
			}
		}

		// No mesh
		if (mesh == nullptr && mesh->GetControlPointsCount() > 0) {
			std::cout << "no mesh found " << m_path << std::endl;
			return false;
		}

		_mesh.SetPath(m_path);
		_mesh.GetGameobject()->onComponentModified.Emmit(&_mesh);

		// Get vertices 
		std::vector<vk::Vertex> & vertices = _mesh.GetVertices();
		FbxVector4*  controlPoints = mesh->GetControlPoints();
		vertices.resize(mesh->GetControlPointsCount());
		for (int ctrlPointIndex = 0; ctrlPointIndex < vertices.size(); ctrlPointIndex++) {
			FbxVector4 point = controlPoints[ctrlPointIndex];
			vertices[ctrlPointIndex].pos = glm::vec3(point[0], point[1], point[2]);
		}

		// Get indices
		std::vector<uint32_t> & indices = _mesh.GetIndices();
		const int* fbxIndices = mesh->GetPolygonVertices();
		indices.resize(mesh->GetPolygonVertexCount());
		for (int vertexIndex = 0; vertexIndex < indices.size(); vertexIndex++) {
			indices[vertexIndex] = fbxIndices[vertexIndex];
		}


		return true;
	}
}
