#include "render/fanGLTFExporter.hpp"
#include "render/resources/fanMesh.hpp"
#include <fstream>
#include "core/fanPath.hpp"
#include "render/fanGLTF.hpp"
#include "core/memory/fanBase64.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool GLTFExporter::Save( const std::string& _path )
    {
        std::ofstream outStream( _path, std::ofstream::out );
        if( outStream.is_open() )
        {
            outStream << mJson;
            outStream.close();
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFExporter::Export( const Mesh& _mesh )
    {
        mJson.clear();
        const std::string    meshName        = Path::FileName( _mesh.mPath );
        const int            indicesCount    = (int)_mesh.mIndices.size();
        const int            vertexCount     = (int)_mesh.mVertices.size();
        const int            offSetPositions = 0;
        const int            offSetNormals   = offSetPositions + vertexCount * sizeof( glm::vec3 );
        const int            offSetTexCoords = offSetNormals + vertexCount * sizeof( glm::vec3 );
        const int            offSetIndices   = offSetTexCoords + vertexCount * sizeof( glm::vec2 );
        const int            bufferSize      = vertexCount * sizeof( glm::vec3 ) + vertexCount * sizeof( glm::vec3 ) + vertexCount * sizeof( glm::vec2 ) + indicesCount * sizeof( unsigned short );
        std::vector<uint8_t> dataBuffer( bufferSize );

        glm::vec3     * arrayPositions = (glm::vec3*)( dataBuffer.data() + offSetPositions );
        glm::vec3     * arrayNormals   = (glm::vec3*)( dataBuffer.data() + offSetNormals );
        glm::vec2     * arrayTexCoords = (glm::vec2*)( dataBuffer.data() + offSetTexCoords );
        unsigned short* arrayIndices   = (unsigned short*)( dataBuffer.data() + offSetIndices );
        for( int i = 0; i < vertexCount; ++i )
        {
            const Vertex& vertex = _mesh.mVertices[i];
            arrayPositions[i] = vertex.mPos;
            arrayNormals[i]   = vertex.mNormal;
            arrayTexCoords[i] = vertex.mUv;
        }
        for( int i = 0; i < indicesCount; ++i )
        {
            arrayIndices[i] = (unsigned short)_mesh.mIndices[i];
        }

        Json& jAsset = mJson["asset"];
        jAsset["version"]   = "2.0";
        jAsset["generator"] = "FanEngine Gltf Exporter";

        Json& jNodes = mJson["nodes"];
        jNodes[0]["name"] = meshName;
        jNodes[0]["mesh"] = 0;

        mJson["scene"] = 0;
        Json& jScenes = mJson["scenes"];
        {
            Json& jScene0 = jScenes[0];
            jScene0["name"]     = "scene0";
            jScene0["nodes"][0] = 0;
        }

        Json& jAccessors = mJson["accessors"];
        {
            GLTFAccessor accessorPositions;
            accessorPositions.mBufferView    = 0;
            accessorPositions.mComponentType = GLTFComponentType::Float;
            accessorPositions.mCount         = vertexCount;
            accessorPositions.mType          = GLTFType::Vec3;
            accessorPositions.Save( jAccessors[0] );

            GLTFAccessor accessorNormals;
            accessorNormals.mBufferView    = 1;
            accessorNormals.mComponentType = GLTFComponentType::Float;
            accessorNormals.mCount         = vertexCount;
            accessorNormals.mType          = GLTFType::Vec3;
            accessorNormals.Save( jAccessors[1] );

            GLTFAccessor accessorTexCoord0;
            accessorTexCoord0.mBufferView    = 2;
            accessorTexCoord0.mComponentType = GLTFComponentType::Float;
            accessorTexCoord0.mCount         = vertexCount;
            accessorTexCoord0.mType          = GLTFType::Vec2;
            accessorTexCoord0.Save( jAccessors[2] );

            GLTFAccessor accessorIndices;
            accessorIndices.mBufferView    = 3;
            accessorIndices.mComponentType = GLTFComponentType::UnsignedShort;
            accessorIndices.mCount         = indicesCount;
            accessorIndices.mType          = GLTFType::Scalar;
            accessorIndices.Save( jAccessors[3] );
        }

        Json& jBufferViews = mJson["bufferViews"];
        {
            GLTFBufferView bufferViewVertices;
            bufferViewVertices.mBuffer     = 0;
            bufferViewVertices.mByteLength = vertexCount * sizeof( glm::vec3 );
            bufferViewVertices.mByteOffset = offSetPositions;
            bufferViewVertices.Save( jBufferViews[0] );

            GLTFBufferView bufferViewNormals;
            bufferViewNormals.mBuffer     = 0;
            bufferViewNormals.mByteLength = vertexCount * sizeof( glm::vec3 );
            bufferViewNormals.mByteOffset = offSetNormals;
            bufferViewNormals.Save( jBufferViews[1] );

            GLTFBufferView bufferViewTexCoord0;
            bufferViewTexCoord0.mBuffer     = 0;
            bufferViewTexCoord0.mByteLength = vertexCount * sizeof( glm::vec2 );
            bufferViewTexCoord0.mByteOffset = offSetTexCoords;
            bufferViewTexCoord0.Save( jBufferViews[2] );

            GLTFBufferView bufferViewIndices;
            bufferViewIndices.mBuffer     = 0;
            bufferViewIndices.mByteLength = indicesCount * sizeof( unsigned short );
            bufferViewIndices.mByteOffset = offSetIndices;
            bufferViewIndices.Save( jBufferViews[3] );
        }

        Json& jBuffers = mJson["buffers"];
        {
            GLTFBuffer buffer;
            buffer.mByteLength = bufferSize;
            buffer.Save( jBuffers[0], "data:application/octet-stream;base64," + Base64::Encode( dataBuffer.data(), (int)dataBuffer.size() ) );
        }

        Json& jMeshes = mJson["meshes"];
        {
            GLTFMesh mesh;
            mesh.mName                  = meshName;
            mesh.mPrimitive0.mPosition  = 0;
            mesh.mPrimitive0.mNormal    = 1;
            mesh.mPrimitive0.mTexCoord0 = 2;
            mesh.mPrimitive0.mIndices   = 3;
            mesh.Save( jMeshes[0] );
        }
    }
}
