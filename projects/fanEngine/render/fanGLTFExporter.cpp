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
        const std::string meshName = Path::FileName( _mesh.mPath );

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

        Json& jMaterials   = mJson["materials"];
        Json& jAccessors   = mJson["accessors"];
        Json& jBufferViews = mJson["bufferViews"];
        Json& jBuffers     = mJson["buffers"];
        Json& jMeshes      = mJson["meshes"];
        int nextAccessor   = 0;
        int nextBufferView = 0;
        int nextBuffer     = 0;

        GLTFMesh mesh;
        mesh.mName = meshName;
        mesh.mPrimitives.resize( _mesh.mSubMeshes.size() );

        for( int primitiveIndex = 0; primitiveIndex < _mesh.mSubMeshes.size(); ++primitiveIndex )
        {
            Json& jMaterial = jMaterials[primitiveIndex];
            jMaterial["name"] = "material" + std::to_string(primitiveIndex);

            const SubMesh& subMesh = _mesh.mSubMeshes[primitiveIndex];
            const int            indicesCount    = (int)subMesh.mIndices.size();
            const int            vertexCount     = (int)subMesh.mVertices.size();
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
                const Vertex& vertex = subMesh.mVertices[i];
                arrayPositions[i] = vertex.mPos;
                arrayNormals[i]   = vertex.mNormal;
                arrayTexCoords[i] = vertex.mUv;
            }
            for( int i = 0; i < indicesCount; ++i )
            {
                arrayIndices[i] = (unsigned short)subMesh.mIndices[i];
            }

            {
                GLTFAccessor accessorPositions;
                accessorPositions.mBufferView    = nextAccessor+0;
                accessorPositions.mComponentType = GLTFComponentType::Float;
                accessorPositions.mCount         = vertexCount;
                accessorPositions.mType          = GLTFType::Vec3;
                accessorPositions.Save( jAccessors[nextAccessor+0] );

                GLTFAccessor accessorNormals;
                accessorNormals.mBufferView    = nextAccessor + 1;
                accessorNormals.mComponentType = GLTFComponentType::Float;
                accessorNormals.mCount         = vertexCount;
                accessorNormals.mType          = GLTFType::Vec3;
                accessorNormals.Save( jAccessors[nextAccessor + 1] );

                GLTFAccessor accessorTexCoord0;
                accessorTexCoord0.mBufferView    = nextAccessor + 2;
                accessorTexCoord0.mComponentType = GLTFComponentType::Float;
                accessorTexCoord0.mCount         = vertexCount;
                accessorTexCoord0.mType          = GLTFType::Vec2;
                accessorTexCoord0.Save( jAccessors[nextAccessor + 2] );

                GLTFAccessor accessorIndices;
                accessorIndices.mBufferView    = nextAccessor + 3;
                accessorIndices.mComponentType = GLTFComponentType::UnsignedShort;
                accessorIndices.mCount         = indicesCount;
                accessorIndices.mType          = GLTFType::Scalar;
                accessorIndices.Save( jAccessors[nextAccessor + 3] );
            }

            {
                GLTFBufferView bufferViewVertices;
                bufferViewVertices.mBuffer     = nextBuffer;
                bufferViewVertices.mByteLength = vertexCount * sizeof( glm::vec3 );
                bufferViewVertices.mByteOffset = offSetPositions;
                bufferViewVertices.Save( jBufferViews[nextBufferView + 0] );

                GLTFBufferView bufferViewNormals;
                bufferViewNormals.mBuffer     = nextBuffer;
                bufferViewNormals.mByteLength = vertexCount * sizeof( glm::vec3 );
                bufferViewNormals.mByteOffset = offSetNormals;
                bufferViewNormals.Save( jBufferViews[nextBufferView + 1] );

                GLTFBufferView bufferViewTexCoord0;
                bufferViewTexCoord0.mBuffer     = nextBuffer;
                bufferViewTexCoord0.mByteLength = vertexCount * sizeof( glm::vec2 );
                bufferViewTexCoord0.mByteOffset = offSetTexCoords;
                bufferViewTexCoord0.Save( jBufferViews[nextBufferView + 2] );

                GLTFBufferView bufferViewIndices;
                bufferViewIndices.mBuffer     = nextBuffer;
                bufferViewIndices.mByteLength = indicesCount * sizeof( unsigned short );
                bufferViewIndices.mByteOffset = offSetIndices;
                bufferViewIndices.Save( jBufferViews[nextBufferView + 3] );
            }

            {
                GLTFBuffer buffer;
                buffer.mByteLength = bufferSize;
                buffer.Save( jBuffers[nextBuffer], "data:application/octet-stream;base64," + Base64::Encode( dataBuffer.data(), (int)dataBuffer.size() ) );
            }

            {
                mesh.mPrimitives[primitiveIndex].mPosition  = nextBufferView + 0;
                mesh.mPrimitives[primitiveIndex].mNormal    = nextBufferView + 1;
                mesh.mPrimitives[primitiveIndex].mTexCoord0 = nextBufferView + 2;
                mesh.mPrimitives[primitiveIndex].mIndices   = nextBufferView + 3;
                mesh.mPrimitives[primitiveIndex].mMaterial  = primitiveIndex;
            }

            nextBufferView += 4;
            nextAccessor += 4;
            nextBuffer += 1;
        }
        mesh.Save( jMeshes[0] );
    }
}
