#include "render/fanGLTFImporter.hpp"
#include "render/fanMesh.hpp"

#include "core/memory/fanBase64.hpp"
#include "core/time/fanScopedTimer.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	GLTFImporter::GLTFPrimitive::GLTFPrimitive(const Json& jPrimitive)
	{
		const Json& jAttribute = jPrimitive["attributes"];
		const bool hasNormal = jAttribute.find("NORMAL") != jAttribute.end();
		const bool hasTexcoord0 = jAttribute.find("TEXCOORD_0") != jAttribute.end();

		indices = jPrimitive["indices"];
		positions = jAttribute["POSITION"];
		normal = hasNormal ? jAttribute["NORMAL"] : -1;
		texcoord0 = hasTexcoord0 ? jAttribute["TEXCOORD_0"] : -1;
	}

	//================================================================================================================================
	//================================================================================================================================
	GLTFImporter::GLTFMesh::GLTFMesh(const Json& jMesh) : primitive0(jMesh["primitives"][0])
	{
		name = jMesh["name"];
	}

	//================================================================================================================================
	//================================================================================================================================
	GLTFImporter::GLTFBufferView::GLTFBufferView(const Json& _jView)
	{
		assert(_jView.find("byteStride") == _jView.end()); // buffers using stride are not supported
		buffer = _jView["buffer"];
		byteLength = _jView["byteLength"];
		byteOffset = _jView["byteOffset"];
	}

	//================================================================================================================================
	//================================================================================================================================
	GLTFImporter::GLTFAccessor::GLTFAccessor(const Json& _jAccessor)
	{
		view = _jAccessor["bufferView"];
		type = _jAccessor["componentType"];
		count = _jAccessor["count"];
		typeStr = _jAccessor["type"];
	}

	//================================================================================================================================
	//================================================================================================================================
	GLTFImporter::GLTFBuffer::GLTFBuffer(const Json& _jBuffer)
	{
		byteLength = _jBuffer["byteLength"];
	}

	//================================================================================================================================
	// Extracts, decodes and returns a segment of buffer from a view
	//================================================================================================================================
	std::string GLTFImporter::GLTFBuffer::GetBuffer(const GLTFBufferView& _view, const std::string& _uri)
	{
		const std::string::const_iterator fullBufferBegin = _uri.begin() + _uri.find_first_of(',') + 1;
		const std::string::const_iterator dataBegin = fullBufferBegin + 4 * _view.byteOffset / 3;
		const std::string::const_iterator dataEnd = dataBegin + 4 * _view.byteLength / 3;
		return Base64::Decode(std::string(dataBegin, dataEnd));
	}

	//================================================================================================================================
	// Parse and load the gltf into a json 
	//================================================================================================================================
	bool GLTFImporter::Load(const std::string _path)
	{
		const std::string extension = std::filesystem::path(_path).extension().string();
		if (extension != ".gltf") 
		{
			Debug::Warning() << "Loading failed, file is not a gltf: " << _path << Debug::Endl();
			return false;
		}

		Debug::Log() << "Loading file : " << _path << Debug::Endl();
		std::ifstream inFile(_path);
		if (inFile.good() == true) 
		{
			m_json.clear();
			inFile >> m_json;
			m_path = _path;
		}
		else 
		{
			Debug::Warning() << "Loading failed: " << _path << Debug::Endl();
		}
		inFile.close();
		return true;
	}

	//================================================================================================================================
	// Converts gltf into a usable mesh
	// For a gltf file we only load the first mesh and the first primitive.
	// All other features are not supported yet
	//================================================================================================================================
	bool GLTFImporter::GetMesh(Mesh& _mesh)
	{
		// Get mesh data references
		std::vector<uint32_t>& meshIndices = _mesh.GetIndices();
		std::vector<Vertex>& meshVertices = _mesh.GetVertices();

		// Get main json lists		
		const Json& jmeshes = m_json["meshes"];
		const Json& jAccessors = m_json["accessors"];
		const Json& jBufferViews = m_json["bufferViews"];
		const Json& jBuffers = m_json["buffers"];

		// Returns if there is no mesh
		if (jmeshes.is_null() || jmeshes.empty()) {
			Debug::Error() << "mesh is empty : " << m_path << Debug::Endl();
			return false;
		}

		GLTFMesh mesh(jmeshes[0]);

		// Loads indices
		GLTFAccessor accessorIndices(jAccessors[mesh.primitive0.indices]);
		GLTFBufferView viewIndices(jBufferViews[accessorIndices.view]);
		GLTFBuffer bufferIndices(jBuffers[viewIndices.buffer]);
		assert(accessorIndices.type == SCALAR);
		std::string indicesBufferStr = bufferIndices.GetBuffer(viewIndices, jBuffers[viewIndices.buffer]["uri"]);
		const unsigned short* indicesArray = (const unsigned short*)indicesBufferStr.data();
		meshIndices.resize(accessorIndices.count);
		for (int i = 0; i < meshIndices.size(); ++i){	meshIndices[i] = indicesArray[i]; }

		// Load positions
		GLTFAccessor accessorPositions(jAccessors[mesh.primitive0.positions]);
		GLTFBufferView viewPositions(jBufferViews[accessorPositions.view]);
		GLTFBuffer bufferPositions(jBuffers[viewPositions.buffer]);
		assert(accessorPositions.type == FLOAT);
		std::string positionsBufferStr = bufferPositions.GetBuffer(viewPositions, jBuffers[viewPositions.buffer]["uri"]);
		const glm::vec3* positionsArray = (const glm::vec3*)positionsBufferStr.data();
		meshVertices.resize(accessorPositions.count);
		for (int i = 0; i < meshVertices.size(); i++){	meshVertices[i].pos = positionsArray[i]; }

		// Load normals
		if (mesh.primitive0.HasNormals() )
		{
			GLTFAccessor accessorNormals(jAccessors[mesh.primitive0.normal]);
			GLTFBufferView viewNormals(jBufferViews[accessorNormals.view]);
			GLTFBuffer bufferNormals(jBuffers[viewNormals.buffer]);
			assert(accessorNormals.type == FLOAT);
			std::string normalsBufferStr = bufferNormals.GetBuffer(viewNormals, jBuffers[viewNormals.buffer]["uri"]);
			const glm::vec3* normalsArray = (const glm::vec3*)normalsBufferStr.data();
			assert( meshVertices.size() == accessorNormals.count );
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].normal = normalsArray[i]; }
		}
		else {
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].normal = glm::vec3(0,0,1); }
		}

		// load textcoords 0
		if (mesh.primitive0.HasTexcoords0() )
		{
			GLTFAccessor accessorTexcoords0(jAccessors[mesh.primitive0.texcoord0]);
			GLTFBufferView viewTexcoords0(jBufferViews[accessorTexcoords0.view]);
			GLTFBuffer bufferPositions(jBuffers[viewTexcoords0.buffer]);
			assert(accessorTexcoords0.type == FLOAT);
			std::string texcoords0BufferStr = bufferPositions.GetBuffer(viewTexcoords0, jBuffers[viewTexcoords0.buffer]["uri"]);
			const glm::vec3* texcoords0Array = (const glm::vec3*)texcoords0BufferStr.data();
			assert(meshVertices.size() == accessorTexcoords0.count);
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].uv = texcoords0Array[i]; }
		}
		else {
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].uv = glm::vec2(0, 0); }
		}
		
		// load colors
		for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].color = glm::vec3(1,1,1); }

		Debug::Warning("test");
		/*

		// Find the first mesh, its first primitive and attributes, then the buffer and buffer views
		const Json& jmesh0 = jmeshes[0];
		const Json& jPrimitive0 = jmesh0["primitives"][0];
		const Json& jAttributes = jPrimitive0["attributes"];
		const Json& jAccessors = m_json["accessors"];
		const Json& jBufferViews = m_json["bufferViews"];
		const Json& jBuffers = m_json["buffers"];

		// Checks attributes existance
		const bool hasNormal = jAttributes.find("NORMAL") != jAttributes.end();
		const bool hasTexcoord0 = jAttributes.find("TEXCOORD_0") != jAttributes.end();

		// Find the accessors
		const Json& indicesAccessor = jAccessors[(int)jPrimitive0["indices"]];
		const Json& positionAccessor = jAccessors[(int)jAttributes["POSITION"]];
		const Json& normalAccessor = hasNormal ? jAccessors[(int)jAttributes["NORMAL"]] : Json();
		const Json& texcoord0Accessor = hasTexcoord0 ? jAccessors[(int)jAttributes["TEXCOORD_0"]] : Json();

		// find the buffer views
		const Json& indicesView = jBufferViews[(int)indicesAccessor["bufferView"]];
		const Json& positionView = jBufferViews[(int)positionAccessor["bufferView"]];
		const Json& normalView = hasNormal ? jBufferViews[(int)normalAccessor["bufferView"]] : Json();
		const Json& texcoord0View = hasTexcoord0 ? jBufferViews[(int)texcoord0Accessor["bufferView"]] : Json();

		// find the buffers
		const Json& indicesBuffer = jBuffers[(int)indicesView["buffer"]];
		const Json& positionBuffer = jBuffers[(int)positionView["buffer"]];
		const Json& normalBuffer = hasNormal ? jBuffers[(int)normalView["buffer"]] : Json();
		const Json& texcoord0Buffer = hasTexcoord0 ? jBuffers[(int)texcoord0View["buffer"]] : Json();

		// Load indices
		{
			const int count = indicesAccessor["count"];
			const int byteLength = indicesView["byteLength"];
			const int byteOffset = indicesView["byteOffset"];
			const int bufferLenght = indicesBuffer["byteLength"];

			assert(indicesView.find("byteStride") == indicesView.end()); // buffers using stride are not supported
			assert(byteLength / count == 2); // indices are ushort
			assert(indicesAccessor["componentType"] == SCALAR);

			const std::string& fullbuffer = indicesBuffer["uri"];
			const std::string::const_iterator fullBufferBegin = fullbuffer.begin() + fullbuffer.find_first_of(',') + 1;
			const std::string::const_iterator dataBegin = fullBufferBegin + 4 * byteOffset / 3;
			const std::string::const_iterator dataEnd = dataBegin + 4 * byteLength / 3;
			const std::string& dataBuffer = Base64::Decode(std::string(dataBegin, dataEnd));

			const unsigned short* indicesArray = (const unsigned short*)dataBuffer.data();

			meshIndices.clear();
			meshIndices.resize(count);
			for (int i = 0; i < meshIndices.size(); ++i)
			{
				meshIndices[i] = indicesArray[i];
			}
			Debug::Warning("test");
		}

		// Load positions
		{
			const int count = positionAccessor["count"];
			const int byteLength = positionView["byteLength"];
			const int byteOffset = positionView["byteOffset"];
			const int bufferLenght = positionBuffer["byteLength"];

			assert(positionView.find("byteStride") == positionView.end()); // buffers using stride are not supported
			assert(byteLength / count == 12); // vertices are vec3
			assert(positionAccessor["componentType"] == FLOAT);

			const std::string& fullbuffer = positionBuffer["uri"];
			const std::string::const_iterator fullBufferBegin = fullbuffer.begin() + fullbuffer.find_first_of(',') + 1;
			const std::string::const_iterator dataBegin = fullBufferBegin + 4 * byteOffset / 3;
			const std::string::const_iterator dataEnd = dataBegin + 4 * byteLength / 3;
			const std::string& dataBuffer = Base64::Decode(std::string(dataBegin, dataEnd));

			const glm::vec3* vec3Array = (const glm::vec3*)dataBuffer.data();

			meshVertices.clear();
			meshVertices.resize(count); // 1 vec3 = 3 float
			for (int i = 0; i < meshVertices.size(); i++)
			{
				meshVertices[i].pos = vec3Array[i];
			}
			Debug::Warning("test");
		}*/

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	void GLTFImporter::GetBufferFromView(std::string& _outStringBuffer) {


	}

	/*fbxsdk::FbxAxisSystem axisSystem(
		fbxsdk::FbxAxisSystem::EUpVector::eYAxis,
		(fbxsdk::FbxAxisSystem::EFrontVector)2,
		fbxsdk::FbxAxisSystem::ECoordSystem::eRightHanded
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
	std::vector<Vertex> & vertices	= _mesh.GetVertices();

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
			vertices[vertexIndex].uv = glm::vec2(1.f - uv[0], uv[1]);

			vertices[vertexIndex].color = glm::vec3(1.f, 1.f, 1.f);
		}

	}*/



	//================================================================================================================================
	//================================================================================================================================
	/*std::vector< fbxsdk::FbxVector4 > FBXImporter::GetNormals(const fbxsdk::FbxMesh * _mesh) {
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

	}*/

	//================================================================================================================================
	//================================================================================================================================
	/*std::vector< fbxsdk::FbxVector2 > FBXImporter::GetUVs(const fbxsdk::FbxMesh * _mesh) {
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
	}*/
}
