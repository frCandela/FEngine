#include <filesystem>
#include <fstream>
#include "core/fanDebug.hpp"
#include "core/memory/fanBase64.hpp"
#include "core/time/fanScopedTimer.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	GLTFImporter::GLTFPrimitive::GLTFPrimitive( const Json& jPrimitive )
	{
		const Json& jAttribute = jPrimitive[ "attributes" ];
		const bool hasNormal = jAttribute.find( "NORMAL" ) != jAttribute.end();
		const bool hasTexcoord0 = jAttribute.find( "TEXCOORD_0" ) != jAttribute.end();

        mIndices   = jPrimitive[ "indices" ];
        mPositions = jAttribute[ "POSITION" ];
        mNormal    = hasNormal ? int( jAttribute["NORMAL"] ) : -1;
        mTexcoord0 = hasTexcoord0 ? int( jAttribute["TEXCOORD_0"] ) : -1;
	}

	//========================================================================================================
	//========================================================================================================
	GLTFImporter::GLTFMesh::GLTFMesh( const Json& jMesh ) : primitive0( jMesh[ "primitives" ][ 0 ] )
	{
		name = jMesh[ "name" ];
	}

	//========================================================================================================
	//========================================================================================================
	GLTFImporter::GLTFBufferView::GLTFBufferView( const Json& _jView )
	{
        fanAssert( _jView.find( "byteStride" ) == _jView.end() ); // buffers using stride are not supported
		mBuffer     = _jView[ "buffer" ];
        mByteLength = _jView[ "byteLength" ];
        mByteOffset = _jView[ "byteOffset" ];
	}

	//========================================================================================================
	//========================================================================================================
	GLTFImporter::GLTFAccessor::GLTFAccessor( const Json& _jAccessor )
	{
        mView    = _jAccessor[ "bufferView" ];
        mType    = _jAccessor[ "componentType" ];
        mCount   = _jAccessor[ "count" ];
        mTypeStr = _jAccessor[ "type" ];
	}

	//========================================================================================================
	//========================================================================================================
	GLTFImporter::GLTFBuffer::GLTFBuffer( const Json& _jBuffer )
	{
        mByteLength = _jBuffer[ "byteLength" ];
	}

	//========================================================================================================
	// Decodes a buffer from base64 to binary
	// there is some header data at the beginnig of the buffer delimited with a ','
	//========================================================================================================
	std::string GLTFImporter::DecodeBuffer( const std::string& _uri )
	{
		const std::string::const_iterator fullBufferBegin = _uri.begin() + _uri.find_first_of( ',' ) + 1;
		return Base64::Decode( std::string( fullBufferBegin, _uri.end() ) );
	}

	//========================================================================================================
	// Extracts, decodes and returns a segment of buffer from a view
	//========================================================================================================
    std::string GLTFImporter::GLTFBuffer::GetBuffer( const GLTFBufferView& _view,
                                                     const std::string& _decodedBuffer ) const
    {
        return std::string( _decodedBuffer.begin() + _view.mByteOffset,
                            _decodedBuffer.begin() + _view.mByteOffset + _view.mByteLength );
	}

	//========================================================================================================
	// Parse and load the gltf into a json 
	//========================================================================================================
	bool GLTFImporter::Load( const std::string _path )
	{
		// Checks extension
		const std::string extension = std::filesystem::path( _path ).extension().string();
		if ( extension != ".gltf" )
		{
			Debug::Warning() << "Loading failed, file is not a gltf: " << _path << Debug::Endl();
			return false;
		}

		// Load the json
		Debug::Log() << "Loading file : " << _path << Debug::Endl();
		std::ifstream inFile( _path );
		if ( !inFile.good() == true )
		{
			Debug::Warning() << "Loading failed: " << _path << Debug::Endl();
			return false;
		}

		mJson.clear();
		inFile >> mJson;
        mPath = _path;
		inFile.close();

		return true;
	}

	//========================================================================================================
	// Converts gltf into a usable mesh
	// For a gltf file we only load the first mesh and the first primitive.
	// All other features are not supported yet
	//========================================================================================================
	bool GLTFImporter::GetMesh( Mesh& _mesh )
	{
		// Get mesh data references
		std::vector<uint32_t>& meshIndices = _mesh.mIndices;
		std::vector<Vertex>& meshVertices = _mesh.mVertices;

		// Get main json lists		
		const Json& jmeshes = mJson[ "meshes" ];
		const Json& jAccessors = mJson[ "accessors" ];
		const Json& jBufferViews = mJson[ "bufferViews" ];
		const Json& jBuffers = mJson[ "buffers" ];
		std::vector< std::string > m_loadedBuffers;
		m_loadedBuffers.resize( jBuffers.size() );

		// Returns if there is no mesh
		if ( jmeshes.is_null() || jmeshes.empty() )
		{
			Debug::Error() << "mesh is empty : " << mPath << Debug::Endl();
			return false;
		}

		// Loads the first mesh json
		GLTFMesh mesh( jmeshes[ 0 ] );

		// Loads indices
		{
			GLTFAccessor accessorIndices( jAccessors[ mesh.primitive0.mIndices ] );
			GLTFBufferView viewIndices( jBufferViews[ accessorIndices.mView ] );
			GLTFBuffer bufferIndices( jBuffers[ viewIndices.mBuffer ] );
            fanAssert( accessorIndices.mType == Scalar );
			if ( m_loadedBuffers[ viewIndices.mBuffer ].empty() )
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[ viewIndices.mBuffer ] = DecodeBuffer( jBuffers[ viewIndices.mBuffer ][ "uri" ] );
			}
			const std::string buffer = bufferIndices.GetBuffer( viewIndices, m_loadedBuffers[ viewIndices.mBuffer ] );
			const unsigned short* indicesArray = ( const unsigned short* ) buffer.data();
			meshIndices.resize( accessorIndices.mCount );
			for ( int i = 0; i < (int)meshIndices.size(); ++i ) { meshIndices[ i ] = indicesArray[ i ]; }
		}

		// Load positions
		{
			GLTFAccessor accessorPositions( jAccessors[ mesh.primitive0.mPositions ] );
			GLTFBufferView viewPositions( jBufferViews[ accessorPositions.mView ] );
			GLTFBuffer bufferPositions( jBuffers[ viewPositions.mBuffer ] );
            fanAssert( accessorPositions.mType == Float );
			if ( m_loadedBuffers[ viewPositions.mBuffer ].empty() )
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[ viewPositions.mBuffer ] = DecodeBuffer( jBuffers[ viewPositions.mBuffer ][ "uri" ] );
			}
			const std::string buffer = bufferPositions.GetBuffer( viewPositions, m_loadedBuffers[ viewPositions.mBuffer ] );
			const glm::vec3* positionsArray = ( const glm::vec3* )buffer.data();
			meshVertices.resize( accessorPositions.mCount );
			for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mPos = positionsArray[ i ]; }
		}

		// Load normals
		if ( mesh.primitive0.HasNormals() )
		{
			GLTFAccessor accessorNormals( jAccessors[ mesh.primitive0.mNormal ] );
			GLTFBufferView viewNormals( jBufferViews[ accessorNormals.mView ] );
			GLTFBuffer bufferNormals( jBuffers[ viewNormals.mBuffer ] );
            fanAssert( accessorNormals.mType == Float );
			if ( m_loadedBuffers[ viewNormals.mBuffer ].empty() )
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[ viewNormals.mBuffer ] = DecodeBuffer( jBuffers[ viewNormals.mBuffer ][ "uri" ] );
			}
			const std::string buffer = bufferNormals.GetBuffer( viewNormals, m_loadedBuffers[ viewNormals.mBuffer ] );
			const glm::vec3* normalsArray = ( const glm::vec3* )buffer.data();
            fanAssert( (int)meshVertices.size() == accessorNormals.mCount );
			for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mNormal = normalsArray[ i ]; }
		}
		else
		{
			for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mNormal = glm::vec3( 0, 0, 1 ); }
		}

		// load textcoords 0
		if ( mesh.primitive0.HasTexcoords0() )
		{
			GLTFAccessor accessorTexcoords0( jAccessors[ mesh.primitive0.mTexcoord0 ] );
			GLTFBufferView viewTexcoords0( jBufferViews[ accessorTexcoords0.mView ] );
			GLTFBuffer bufferTexcoord0( jBuffers[ viewTexcoords0.mBuffer ] );
            fanAssert( accessorTexcoords0.mType == Float );
			if ( m_loadedBuffers[ viewTexcoords0.mBuffer ].empty() )
			{ // Load the buffer if it's not loaded already
				m_loadedBuffers[ viewTexcoords0.mBuffer ] = DecodeBuffer( jBuffers[ viewTexcoords0.mBuffer ][ "uri" ] );
			}
			const std::string buffer = bufferTexcoord0.GetBuffer( viewTexcoords0, m_loadedBuffers[ viewTexcoords0.mBuffer ] );
			const glm::vec2* texcoords0Array = ( const glm::vec2* )buffer.data();
            fanAssert( (int)meshVertices.size() == accessorTexcoords0.mCount );
			for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mUv = texcoords0Array[ i ]; }
		}
		else
		{
			for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mUv = glm::vec2( 0, 0 ); }
		}

		// load colors
		for ( int i = 0; i < (int)meshVertices.size(); i++ ) { meshVertices[ i ].mColor = glm::vec3( 1, 1, 1 ); }

		return true;
	}
}
