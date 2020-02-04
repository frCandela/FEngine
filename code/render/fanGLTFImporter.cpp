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
	// Decodes a buffer from base64 to binary
	// there is some header data at the beginnig of the buffer delimited with a ','
	//================================================================================================================================
	std::string GLTFImporter::DecodeBuffer(const std::string& _uri ) {
		const std::string::const_iterator fullBufferBegin = _uri.begin() + _uri.find_first_of(',') + 1;
		return Base64::Decode(std::string(fullBufferBegin, _uri.end()));
	}

	//================================================================================================================================
	// Extracts, decodes and returns a segment of buffer from a view
	//================================================================================================================================
	std::string GLTFImporter::GLTFBuffer::GetBuffer(const GLTFBufferView& _view, const std::string& _decodedBuffer) const
	{
		return std::string(_decodedBuffer.begin() + _view.byteOffset, _decodedBuffer.begin() + _view.byteOffset + _view.byteLength);
	}

	//================================================================================================================================
	// Parse and load the gltf into a json 
	//================================================================================================================================
	bool GLTFImporter::Load(const std::string _path)
	{
		// Checks extension
		const std::string extension = std::filesystem::path(_path).extension().string();
		if (extension != ".gltf") 
		{
			Debug::Warning() << "Loading failed, file is not a gltf: " << _path << Debug::Endl();
			return false;
		}

		// Load the json
		Debug::Log() << "Loading file : " << _path << Debug::Endl();
		std::ifstream inFile(_path);
		if (!inFile.good() == true)
		{
			Debug::Warning() << "Loading failed: " << _path << Debug::Endl();
			return false;
		}

		m_json.clear();
		inFile >> m_json;
		m_path = _path;
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
		std::vector< std::string > m_loadedBuffers;
		m_loadedBuffers.resize(jBuffers.size());

		// Returns if there is no mesh
		if (jmeshes.is_null() || jmeshes.empty()) {
			Debug::Error() << "mesh is empty : " << m_path << Debug::Endl();
			return false;
		}

		// Loads the first mesh json
		GLTFMesh mesh(jmeshes[0]);

		// Loads indices
		{
			GLTFAccessor accessorIndices(jAccessors[mesh.primitive0.indices]);
			GLTFBufferView viewIndices(jBufferViews[accessorIndices.view]);
			GLTFBuffer bufferIndices(jBuffers[viewIndices.buffer]);
			assert(accessorIndices.type == SCALAR);
			if (m_loadedBuffers[viewIndices.buffer].empty())
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[viewIndices.buffer] = DecodeBuffer(jBuffers[viewIndices.buffer]["uri"]);
			}
			const std::string buffer = bufferIndices.GetBuffer(viewIndices, m_loadedBuffers[viewIndices.buffer]);
			const unsigned short* indicesArray = (const unsigned short*)buffer.data();
			meshIndices.resize(accessorIndices.count);
			for (int i = 0; i < meshIndices.size(); ++i) { meshIndices[i] = indicesArray[i]; }
		}

		// Load positions
		{
			GLTFAccessor accessorPositions(jAccessors[mesh.primitive0.positions]);
			GLTFBufferView viewPositions(jBufferViews[accessorPositions.view]);
			GLTFBuffer bufferPositions(jBuffers[viewPositions.buffer]);
			assert(accessorPositions.type == FLOAT);
			if (m_loadedBuffers[viewPositions.buffer].empty())
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[viewPositions.buffer] = DecodeBuffer(jBuffers[viewPositions.buffer]["uri"]);
			}
			const std::string buffer = bufferPositions.GetBuffer(viewPositions, m_loadedBuffers[viewPositions.buffer]);
			const glm::vec3* positionsArray = (const glm::vec3*)buffer.data();
			meshVertices.resize(accessorPositions.count);
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].pos = positionsArray[i]; }
		}

		// Load normals
		if (mesh.primitive0.HasNormals() )
		{
			GLTFAccessor accessorNormals(jAccessors[mesh.primitive0.normal]);
			GLTFBufferView viewNormals(jBufferViews[accessorNormals.view]);
			GLTFBuffer bufferNormals(jBuffers[viewNormals.buffer]);
			assert(accessorNormals.type == FLOAT);
			if (m_loadedBuffers[viewNormals.buffer].empty())
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[viewNormals.buffer] = DecodeBuffer(jBuffers[viewNormals.buffer]["uri"]);
			}
			const std::string buffer = bufferNormals.GetBuffer(viewNormals, m_loadedBuffers[viewNormals.buffer]);
			const glm::vec3* normalsArray = (const glm::vec3*)buffer.data();
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
			GLTFBuffer bufferTexcoord0(jBuffers[viewTexcoords0.buffer]);
			assert(accessorTexcoords0.type == FLOAT);
			if (m_loadedBuffers[viewTexcoords0.buffer].empty())
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[viewTexcoords0.buffer] = DecodeBuffer(jBuffers[viewTexcoords0.buffer]["uri"]);
			}
			const std::string buffer = bufferTexcoord0.GetBuffer(viewTexcoords0, m_loadedBuffers[viewTexcoords0.buffer]);
			const glm::vec2* texcoords0Array = (const glm::vec2*)buffer.data();
			assert(meshVertices.size() == accessorTexcoords0.count);
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].uv = texcoords0Array[i]; }
		}
		else {
			for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].uv = glm::vec2(0, 0); }
		}
		
		// load colors
		for (int i = 0; i < meshVertices.size(); i++) { meshVertices[i].color = glm::vec3(1,1,1); }

		return true;
	}
}
