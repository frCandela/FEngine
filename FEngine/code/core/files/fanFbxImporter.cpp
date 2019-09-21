#include "fanGlobalIncludes.h"

#include "core/files/fanFbxImporter.h"
#include "renderer/fanMesh.h"
#include "scene/fanEntity.h"
#include "fbxsdk.h"


namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	FBXImporter::FBXImporter() {

		//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
		m_sdkManager = FbxManager::Create();
		if (m_sdkManager == nullptr ) {
			Debug::Error( "Error: Unable to create FBX Manager!\n" );
		} else {
			//Debug::Get() << Debug::Severity::log << "Autodesk FBX SDK version" << m_sdkManager->GetVersion() << Debug::Endl();
		}

		// fbx manager
		FbxIOSettings* ios = FbxIOSettings::Create(m_sdkManager, IOSROOT);
		m_sdkManager->SetIOSettings(ios);
		FbxString path = FbxGetApplicationDirectory();
		m_sdkManager->LoadPluginsDirectory(path.Buffer());

		// fbx scene
		m_scene = FbxScene::Create(m_sdkManager, "fbx scene");
		if (m_scene == nullptr ) {
			Debug::Error( "Error: Unable to create FBX scene!\n" );
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
			Debug::Error( "Call to FbxImporter::Initialize() failed." );
			Debug::Get() << Debug::Severity::error << "Error returned: " <<  error.Buffer()  << " " << m_path << Debug::Endl();

			if (m_importer->GetStatus().GetCode() == fbxsdk::FbxStatus::eInvalidFileVersion)
			{
				Debug::Get() << Debug::Severity::log << "FBX file format version for this FBX SDK is " << m_SDKMajor << "." << m_SDKMinor << "." << m_SDKRevision << Debug::Endl();
				Debug::Get() << Debug::Severity::log << "FBX file format version for file " << m_path << " is " << fileMajor << "." << fileMinor << "." << fileRevision << Debug::Endl();
			}
			return false;
		}

		if (m_importer->IsFBX() == false) {
			Debug::Warning( "Error: File is not an fbx" );
			return false;
		}

		bool status = m_importer->Import( m_scene );
		if (status == false ) {
			if (m_importer->GetStatus().GetCode() == fbxsdk::FbxStatus::ePasswordError) {
				Debug::Warning( "password is wrong" );
			}
			Debug::Error( "import failed" );
			return false;
		}

		Debug::Get() << Debug::Severity::log << "successfully imported " << m_path << Debug::Endl();

 		fbxsdk::FbxGeometryConverter geometryConverter( m_sdkManager );
 		geometryConverter.Triangulate(m_scene, true);

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool FBXImporter::GetMesh( Mesh & _mesh) {
		
		fbxsdk::FbxAxisSystem axisSystem( 
			fbxsdk::FbxAxisSystem::EUpVector::eYAxis, 
			(fbxsdk::FbxAxisSystem::EFrontVector)-2, 
			fbxsdk::FbxAxisSystem::ECoordSystem::eLeftHanded
		);
		axisSystem.ConvertScene(m_scene);

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
		if (mesh == nullptr || mesh->GetControlPointsCount() == 0) {
			Debug::Get() << Debug::Severity::error << "no mesh found in" << m_path << Debug::Endl();
			return false;
		}

		std::vector<uint32_t>	& indices	= _mesh.GetIndices();
		std::vector<vk::Vertex> & vertices	= _mesh.GetVertices();

		const fbxsdk::FbxAMatrix & globalTransform = mesh->GetNode()->EvaluateGlobalTransform();
		fbxsdk::FbxAMatrix globalRotation = globalTransform;
		globalRotation.SetT(fbxsdk::FbxVector4(0, 0, 0));

		const FbxVector4* const  controlPoints = mesh->GetControlPoints();
		const int polygonCount = mesh->GetPolygonCount();

		const std::vector< fbxsdk::FbxVector2 > uvs = GetUVs(mesh);
		const std::vector< fbxsdk::FbxVector4 > normals = GetNormals( mesh );
		if (normals.size() == 0) { return false; }

		indices.resize(3*polygonCount);
		vertices.resize(3*polygonCount);
		for (int polyIndex = 0; polyIndex < polygonCount; polyIndex++) {
			for (int inPolyIndex = 0; inPolyIndex < 3; inPolyIndex++) {
				const int vertexIndex = 3 * polyIndex + inPolyIndex;
				const int controlPointIndex = mesh->GetPolygonVertex(polyIndex, inPolyIndex);

				// Fake index
				indices[vertexIndex] = vertexIndex;

				// Vertex
				FbxVector4 point = controlPoints[controlPointIndex];
				point = globalTransform.MultT(point);
				vertices[vertexIndex].pos = glm::vec3(point[0], point[1], point[2]);

				// Normal
				fbxsdk::FbxQuaternion quat;
				globalTransform.MultQ(quat);
				fbxsdk::FbxVector4 normal = normals[vertexIndex];
				normal = globalRotation.MultT(normal);
				vertices[vertexIndex].normal = glm::vec3(normal[0], normal[1], normal[2]);

				// Uv
				const fbxsdk::FbxVector2& uv = uvs[vertexIndex];
				vertices[vertexIndex].uv = glm::vec2(uv[0], uv[1]);
			}

		}
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< fbxsdk::FbxVector4 > FBXImporter::GetNormals(const fbxsdk::FbxMesh * _mesh) {
		const fbxsdk::FbxGeometryElementNormal * elementNormal = _mesh->GetElementNormal();
		const fbxsdk::FbxLayerElement::EMappingMode mappingMode = elementNormal->GetMappingMode();
		const fbxsdk::FbxLayerElement::EReferenceMode referenceMode = elementNormal->GetReferenceMode();
		const int vertexCount = _mesh->GetPolygonVertexCount();

		std::vector< fbxsdk::FbxVector4 > normals;
		normals.reserve(vertexCount);

		const fbxsdk::FbxLayerElementArrayTemplate< fbxsdk::FbxVector4 > & normalsArray = elementNormal->GetDirectArray();
		if ( mappingMode == fbxsdk::FbxLayerElement::eByPolygonVertex) {
			if ( referenceMode == fbxsdk::FbxLayerElement::eDirect ) {						
				for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
					normals.push_back(normalsArray[vertexIndex]);
				}
			} else if ( referenceMode == fbxsdk::FbxLayerElement::eIndexToDirect ) {
				const fbxsdk::FbxLayerElementArrayTemplate<int>& normalsIndexes = elementNormal->GetIndexArray();
				for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
					const int index = normalsIndexes.GetAt(vertexIndex);
					normals.push_back(normalsArray.GetAt(index));
				}
			} else  {
				Debug::Error( "unknown reference mode for normals" );
			}
		} else if( mappingMode == fbxsdk::FbxLayerElement::eByControlPoint ){
			if (referenceMode == fbxsdk::FbxLayerElement::eDirect) {
				const int* const fbxIndices = _mesh->GetPolygonVertices();
				for (int vertexIndex = 0; vertexIndex < vertexCount; vertexIndex++) {
					normals.push_back(normalsArray.GetAt(fbxIndices[vertexIndex]));
				}
			}
			else {
				Debug::Error("unknown reference mode for normals");
			}
		} else {
			Debug::Error( "unknown mapping mode for normals");
		}

		return normals;

	}

	//================================================================================================================================
	//================================================================================================================================
	std::vector< fbxsdk::FbxVector2 > FBXImporter::GetUVs(const fbxsdk::FbxMesh * _mesh) {
		const fbxsdk::FbxGeometryElementUV * elementUV = _mesh->GetElementUV();
		const fbxsdk::FbxLayerElementArrayTemplate<fbxsdk::FbxVector2> & uvsArray = elementUV->GetDirectArray();
		const fbxsdk::FbxLayerElement::EMappingMode mappingMode = elementUV->GetMappingMode();
		const fbxsdk::FbxLayerElement::EReferenceMode referenceMode = elementUV->GetReferenceMode();

		const int polygonCount = _mesh->GetPolygonCount();
		const int vertexCount = _mesh->GetPolygonVertexCount();
		std::vector< fbxsdk::FbxVector2 > uvs;
		uvs.reserve(vertexCount);

		switch ( mappingMode ) {
			case fbxsdk::FbxLayerElement::eByPolygonVertex: {
				switch (referenceMode)
				{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect: {
						for (int polyIndex = 0; polyIndex < polygonCount; polyIndex++) {
							for (int inPolyIndex = 0; inPolyIndex < 3; inPolyIndex++) {
								const int uvIndex = const_cast<fbxsdk::FbxMesh *>(_mesh)->GetTextureUVIndex(polyIndex, inPolyIndex); // Fbx sdk's fault :3
								uvs.push_back(uvsArray.GetAt(uvIndex));
							}
						}
					} break;
					default: {
						Debug::Error("unknown reference mode for uvs");
					} break;
				}
			} break;
			default: {
				Debug::Error("unknown mapping mode for uvs");
			} break;
		}
		return uvs;
	}
}
