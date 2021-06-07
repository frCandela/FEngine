#include <fstream>
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanSkinnedMesh.hpp"
#include "render/fanGLTF.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // Parse and load the gltf into a json
    // Preprocess buffers into an easy to read state
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

        // Get main json lists
        const Json& jMeshes      = mJson["meshes"];
        const Json& jAccessors   = mJson["accessors"];
        const Json& jBufferViews = mJson["bufferViews"];
        const Json& jBuffers     = mJson["buffers"];
        std::vector<std::string> m_loadedBuffers;
        m_loadedBuffers.resize( jBuffers.size() );

        // Returns if there is no mesh
        if( jMeshes.is_null() || jMeshes.empty() )
        {
            Debug::Error() << "mesh is empty : " << mPath << Debug::Endl();
            return false;
        }

        // Loads the first mesh json
        GLTFMesh mesh;
        mesh.Load( jMeshes[0] );

        mSubmeshes.resize( mesh.mPrimitives.size() );
        for( int primitiveIndex = 0; primitiveIndex < mesh.mPrimitives.size(); ++primitiveIndex )
        {
            GLTFSubmeshData& submesh = mSubmeshes[primitiveIndex];

            // Loads indices
            {
                GLTFAccessor accessorIndices;
                accessorIndices.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mIndices] );
                GLTFBufferView viewIndices;
                viewIndices.Load( jBufferViews[accessorIndices.mBufferView] );
                GLTFBuffer bufferIndices;
                bufferIndices.Load( jBuffers[viewIndices.mBuffer] );
                fanAssert( accessorIndices.mComponentType == GLTFComponentType::UnsignedShort );
                if( m_loadedBuffers[viewIndices.mBuffer].empty() )
                {
                    m_loadedBuffers[viewIndices.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewIndices.mBuffer]["uri"] );
                }
                submesh.indexBuffer  = bufferIndices.GetBuffer( viewIndices, m_loadedBuffers[viewIndices.mBuffer] );
                submesh.indicesArray = (unsigned short*)submesh.indexBuffer.data();
                submesh.indicesCount = accessorIndices.mCount;
            }

            // Load positions
            {
                GLTFAccessor accessorPositions;
                accessorPositions.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mPosition] );
                GLTFBufferView viewPositions;
                viewPositions.Load( jBufferViews[accessorPositions.mBufferView] );
                GLTFBuffer bufferPositions;
                bufferPositions.Load( jBuffers[viewPositions.mBuffer] );
                fanAssert( accessorPositions.mComponentType == GLTFComponentType::Float );
                if( m_loadedBuffers[viewPositions.mBuffer].empty() )
                {
                    m_loadedBuffers[viewPositions.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewPositions.mBuffer]["uri"] );
                }
                submesh.posBuffer      = bufferPositions.GetBuffer( viewPositions, m_loadedBuffers[viewPositions.mBuffer] );
                submesh.positionsArray = (glm::vec3*)submesh.posBuffer.data();
                submesh.verticesCount  = accessorPositions.mCount;
            }

            // Load normals
            if( mesh.mPrimitives[primitiveIndex].HasNormals() )
            {
                GLTFAccessor accessorNormals;
                accessorNormals.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mNormal] );
                GLTFBufferView viewNormals;
                viewNormals.Load( jBufferViews[accessorNormals.mBufferView] );
                GLTFBuffer bufferNormals;
                bufferNormals.Load( jBuffers[viewNormals.mBuffer] );
                fanAssert( accessorNormals.mComponentType == GLTFComponentType::Float );
                if( m_loadedBuffers[viewNormals.mBuffer].empty() )
                {
                    m_loadedBuffers[viewNormals.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewNormals.mBuffer]["uri"] );
                }
                submesh.normalsBuffer = bufferNormals.GetBuffer( viewNormals, m_loadedBuffers[viewNormals.mBuffer] );
                submesh.normalsArray  = (glm::vec3*)submesh.normalsBuffer.data();
                fanAssert( submesh.verticesCount == accessorNormals.mCount );
            }

            // load textcoords 0
            if( mesh.mPrimitives[primitiveIndex].HasTexcoords0() )
            {
                GLTFAccessor accessorTexcoords0;
                accessorTexcoords0.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mTexCoord0] );
                GLTFBufferView viewTexcoords0;
                viewTexcoords0.Load( jBufferViews[accessorTexcoords0.mBufferView] );
                GLTFBuffer bufferTexcoord0;
                bufferTexcoord0.Load( jBuffers[viewTexcoords0.mBuffer] );
                fanAssert( accessorTexcoords0.mComponentType == GLTFComponentType::Float );
                if( m_loadedBuffers[viewTexcoords0.mBuffer].empty() )
                {
                    m_loadedBuffers[viewTexcoords0.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewTexcoords0.mBuffer]["uri"] );
                }
                submesh.texcoords0Buffer = bufferTexcoord0.GetBuffer( viewTexcoords0, m_loadedBuffers[viewTexcoords0.mBuffer] );
                submesh.texcoords0Array  = (glm::vec2*)submesh.texcoords0Buffer.data();
                fanAssert( submesh.verticesCount == accessorTexcoords0.mCount );
            }
        }
        return true;
    }

    //==================================================================================================================================================================================================
    // Set _mesh with the previously imported data
    //==================================================================================================================================================================================================
    void GLTFImporter::GetMesh( Mesh& _mesh )
    {
        fanAssert( _mesh.mSubMeshes.empty() );
        _mesh.mSubMeshes.resize( mSubmeshes.size() );

        for( int primitiveIndex = 0; primitiveIndex < mSubmeshes.size(); ++primitiveIndex )
        {
            const GLTFSubmeshData& submesh     = mSubmeshes[primitiveIndex];
            std::vector<uint32_t>& meshIndices = _mesh.mSubMeshes[primitiveIndex].mIndices;
            meshIndices.resize( submesh.indicesCount );
            std::vector<Vertex>& meshVertices = _mesh.mSubMeshes[primitiveIndex].mVertices;
            meshVertices.resize( submesh.verticesCount );

            // set indices
            for( int i = 0; i < (int)meshIndices.size(); ++i )
            {
                meshIndices[i] = submesh.indicesArray[i];
            }

            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mPos    = submesh.positionsArray[i];
                meshVertices[i].mNormal = submesh.normalsArray != nullptr ? submesh.normalsArray[i] : glm::vec3( 0, 0, 1 );
                meshVertices[i].mColor  = Color::sWhite.ToGLM3();
                meshVertices[i].mUv     = submesh.texcoords0Array != nullptr ? submesh.texcoords0Array[i] : glm::vec2( 0, 0 );
            }
        }
    }

    //==================================================================================================================================================================================================
    // Set _mesh with the previously imported data
    //==================================================================================================================================================================================================
    void GLTFImporter::GetMesh( SkinnedMesh& _mesh )
    {
        fanAssert( _mesh.mSubMeshes.empty() );
        _mesh.mSubMeshes.resize( mSubmeshes.size() );

        for( int primitiveIndex = 0; primitiveIndex < mSubmeshes.size(); ++primitiveIndex )
        {
            const GLTFSubmeshData& submesh     = mSubmeshes[primitiveIndex];
            std::vector<uint32_t>& meshIndices = _mesh.mSubMeshes[primitiveIndex].mIndices;
            meshIndices.resize( submesh.indicesCount );
            std::vector<VertexSkinned>& meshVertices = _mesh.mSubMeshes[primitiveIndex].mVertices;
            meshVertices.resize( submesh.verticesCount );

            // set indices
            for( int i = 0; i < (int)meshIndices.size(); ++i )
            {
                meshIndices[i] = submesh.indicesArray[i];
            }

            for( int i = 0; i < (int)meshVertices.size(); i++ )
            {
                meshVertices[i].mPos    = submesh.positionsArray[i];
                meshVertices[i].mNormal = submesh.normalsArray != nullptr ? submesh.normalsArray[i] : glm::vec3( 0, 0, 1 );
                meshVertices[i].mColor  = Color::sWhite.ToGLM3();
                meshVertices[i].mUv     = submesh.texcoords0Array != nullptr ? submesh.texcoords0Array[i] : glm::vec2( 0, 0 );
            }
        }
    }
}
