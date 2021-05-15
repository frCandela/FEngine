#include <fstream>
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanGLTF.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Parse and load the gltf into a json
    //==================================================================================================================================================================================================
    bool GLTFImporter::Load( const std::string& _path )
    {
        // Checks extension
        const std::string extension = Path::Extension( _path );
        if( extension != "gltf" )
        {
            Debug::Warning() << "Loading failed, file is not a gltf: " << _path << Debug::Endl();
            return false;
        }

        // Load the json
        Debug::Log() << "Loading file : " << _path << Debug::Endl();
        std::ifstream inFile( _path );
        if( !inFile.good() == true )
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

    //==================================================================================================================================================================================================
    // Converts gltf into a usable mesh
    // For a gltf file we only load the first mesh and the first primitive.
    // All other features are not supported yet
    //==================================================================================================================================================================================================
    bool GLTFImporter::GetMesh( Mesh& _mesh )
    {
        // Get mesh data references
        std::vector<uint32_t>& meshIndices  = _mesh.mIndices;
        std::vector<Vertex>  & meshVertices = _mesh.mVertices;

        // Get main json lists
        const Json& jmeshes      = mJson["meshes"];
        const Json& jAccessors   = mJson["accessors"];
        const Json& jBufferViews = mJson["bufferViews"];
        const Json& jBuffers     = mJson["buffers"];
        std::vector<std::string> m_loadedBuffers;
        m_loadedBuffers.resize( jBuffers.size() );

        // Returns if there is no mesh
        if( jmeshes.is_null() || jmeshes.empty() )
        {
            Debug::Error() << "mesh is empty : " << mPath << Debug::Endl();
            return false;
        }

        // Loads the first mesh json
        GLTFMesh mesh;
        mesh.Load( jmeshes[0] );

        // Loads indices
        {
            GLTFAccessor accessorIndices;
            accessorIndices.Load( jAccessors[mesh.mPrimitive0.mIndices] );
            GLTFBufferView viewIndices;
            viewIndices.Load( jBufferViews[accessorIndices.mBufferView] );
            GLTFBuffer bufferIndices;
            bufferIndices.Load( jBuffers[viewIndices.mBuffer] );
            fanAssert( accessorIndices.mComponentType == GLTFComponentType::UnsignedShort );
            if( m_loadedBuffers[viewIndices.mBuffer].empty() )
            {
                m_loadedBuffers[viewIndices.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewIndices.mBuffer]["uri"] );
            }
            const std::string buffer = bufferIndices.GetBuffer( viewIndices, m_loadedBuffers[viewIndices.mBuffer] );
            const unsigned short* indicesArray = (const unsigned short*)buffer.data();
            meshIndices.resize( accessorIndices.mCount );
            for( int i = 0; i < (int)meshIndices.size(); ++i )
            {
                meshIndices[i] = indicesArray[i];
            }
        }

        // Load positions
        {
            GLTFAccessor accessorPositions;
            accessorPositions.Load( jAccessors[mesh.mPrimitive0.mPosition] );
            GLTFBufferView viewPositions;
            viewPositions.Load( jBufferViews[accessorPositions.mBufferView] );
            GLTFBuffer bufferPositions;
            bufferPositions.Load( jBuffers[viewPositions.mBuffer] );
            fanAssert( accessorPositions.mComponentType == GLTFComponentType::Float );
            if( m_loadedBuffers[viewPositions.mBuffer].empty() )
            {
                m_loadedBuffers[viewPositions.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewPositions.mBuffer]["uri"] );
            }
            const std::string buffer = bufferPositions.GetBuffer( viewPositions, m_loadedBuffers[viewPositions.mBuffer] );
            const glm::vec3* positionsArray = (const glm::vec3*)buffer.data();
            meshVertices.resize( accessorPositions.mCount );
            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mPos = positionsArray[i];
            }
        }

        // Load normals
        if( mesh.mPrimitive0.HasNormals() )
        {
            GLTFAccessor accessorNormals;
            accessorNormals.Load( jAccessors[mesh.mPrimitive0.mNormal] );
            GLTFBufferView viewNormals;
            viewNormals.Load( jBufferViews[accessorNormals.mBufferView] );
            GLTFBuffer bufferNormals;
            bufferNormals.Load( jBuffers[viewNormals.mBuffer] );
            fanAssert( accessorNormals.mComponentType == Float );
            if( m_loadedBuffers[viewNormals.mBuffer].empty() )
            {
                m_loadedBuffers[viewNormals.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewNormals.mBuffer]["uri"] );
            }
            const std::string buffer = bufferNormals.GetBuffer( viewNormals, m_loadedBuffers[viewNormals.mBuffer] );
            const glm::vec3* normalsArray = (const glm::vec3*)buffer.data();
            fanAssert( (int)meshVertices.size() == accessorNormals.mCount );
            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mNormal = normalsArray[i];
            }
        }
        else
        {
            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mNormal = glm::vec3( 0, 0, 1 );
            }
        }

        // load textcoords 0
        if( mesh.mPrimitive0.HasTexcoords0() )
        {
            GLTFAccessor accessorTexcoords0;
            accessorTexcoords0.Load( jAccessors[mesh.mPrimitive0.mTexCoord0] );
            GLTFBufferView viewTexcoords0;
            viewTexcoords0.Load( jBufferViews[accessorTexcoords0.mBufferView] );
            GLTFBuffer bufferTexcoord0;
            bufferTexcoord0.Load( jBuffers[viewTexcoords0.mBuffer] );
            fanAssert( accessorTexcoords0.mComponentType == GLTFComponentType::Float );
            if( m_loadedBuffers[viewTexcoords0.mBuffer].empty() )
            {
                m_loadedBuffers[viewTexcoords0.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewTexcoords0.mBuffer]["uri"] );
            }
            const std::string buffer = bufferTexcoord0.GetBuffer( viewTexcoords0, m_loadedBuffers[viewTexcoords0.mBuffer] );
            const glm::vec2* texcoords0Array = (const glm::vec2*)buffer.data();
            fanAssert( (int)meshVertices.size() == accessorTexcoords0.mCount );
            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mUv = texcoords0Array[i];
            }
        }
        else
        {
            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mUv = glm::vec2( 0, 0 );
            }
        }

        // load colors
        for( int i = 0; i < (int)meshVertices.size(); i++ )
        {
            meshVertices[i].mColor = glm::vec3( 1, 1, 1 );
        }

        return true;
    }
}
