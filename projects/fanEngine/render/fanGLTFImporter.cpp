#include <fstream>
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanGLTF.hpp"
#include <stack>

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
        const Json& jSkins       = mJson["skins"];
        const Json& jNodes       = mJson["nodes"];
        const Json& jScenes      = mJson["scenes"];
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

        // loads scenes
        std::vector<GLTFScene> scenes;
        if( jScenes.is_array() && jScenes.size() != 0 )
        {
            scenes.resize( jScenes.size() );
            for( int i = 0; i < jScenes.size(); i++ )
            {
                scenes[i].Load( jScenes[i] );
            }
        }

        // load nodes
        std::vector<GLTFNode> nodes;
        if( jNodes.is_array() && jNodes.size() != 0 )
        {
            nodes.resize( jNodes.size() );
            for( int i = 0; i < jNodes.size(); i++ )
            {
                nodes[i].Load( jNodes[i] );
            }
        }

        // set parents of all nodes
        for( int parentIndex = 0; parentIndex < nodes.size(); ++parentIndex )
        {
            GLTFNode& parent = nodes[parentIndex];
            for( int childIndex : parent.mChildren )
            {
                GLTFNode& child = nodes[childIndex];
                child.mParent = parentIndex;
            }
        }

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

            // load joints0
            if( mesh.mPrimitives[primitiveIndex].HasJoints0() )
            {
                GLTFAccessor accessorJoints0;
                accessorJoints0.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mJoints0] );
                fanAssert( accessorJoints0.mType == GLTFType::Vec4 );
                fanAssert( accessorJoints0.mComponentType == GLTFComponentType::UnsignedByte );
                fanAssert( submesh.verticesCount == accessorJoints0.mCount );
                GLTFBufferView viewJoints0;
                viewJoints0.Load( jBufferViews[accessorJoints0.mBufferView] );
                GLTFBuffer bufferJoints0;
                bufferJoints0.Load( jBuffers[viewJoints0.mBuffer] );

                if( m_loadedBuffers[viewJoints0.mBuffer].empty() )
                {
                    m_loadedBuffers[viewJoints0.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewJoints0.mBuffer]["uri"] );
                }
                submesh.joints0Buffer = bufferJoints0.GetBuffer( viewJoints0, m_loadedBuffers[viewJoints0.mBuffer] );
                submesh.joints0Array  = (glm::u8vec4*)submesh.joints0Buffer.data();
            }

            // load weights0
            if( mesh.mPrimitives[primitiveIndex].HasWeights0() )
            {
                GLTFAccessor accessorWeights0;
                accessorWeights0.Load( jAccessors[mesh.mPrimitives[primitiveIndex].mWeights0] );
                fanAssert( accessorWeights0.mType == GLTFType::Vec4 );
                fanAssert( accessorWeights0.mComponentType == GLTFComponentType::Float );
                fanAssert( submesh.verticesCount == accessorWeights0.mCount );
                GLTFBufferView viewWeight0;
                viewWeight0.Load( jBufferViews[accessorWeights0.mBufferView] );
                GLTFBuffer bufferWeights0;
                bufferWeights0.Load( jBuffers[viewWeight0.mBuffer] );

                if( m_loadedBuffers[viewWeight0.mBuffer].empty() )
                {
                    m_loadedBuffers[viewWeight0.mBuffer] = GLTFBuffer::DecodeBuffer( jBuffers[viewWeight0.mBuffer]["uri"] );
                }
                submesh.weights0Buffer = bufferWeights0.GetBuffer( viewWeight0, m_loadedBuffers[viewWeight0.mBuffer] );
                submesh.weights0Array  = (glm::vec4*)submesh.weights0Buffer.data();
            }

            // loads skins
            if( jSkins.is_array() && jSkins.size() != 0 )
            {
                GLTFSkin skin;
                skin.Load( jSkins[0] );
                submesh.mSkeleton.mName     = skin.mName;
                submesh.mSkeleton.mNumBones = (int)skin.mJoints.size();

                std::vector<GLTFNode> skeletonNodes;
                skeletonNodes.resize( skin.mJoints.size() );
                fanAssert( skin.mJoints.size() <= RenderGlobal::sMaxBones );
                std::map<int, int> remapTable;
                for( int           i   = 0; i < skin.mJoints.size(); i++ )
                {
                    const int nodeIndex = skin.mJoints[i];
                    const GLTFNode& node = nodes[nodeIndex];
                    remapTable[nodeIndex] = i;
                    skeletonNodes[i]      = node;
                }

                for( int nodeIndex = 0; nodeIndex < skeletonNodes.size(); nodeIndex++ )
                {
                    const GLTFNode& node = skeletonNodes[nodeIndex];
                    Bone          & bone = submesh.mSkeleton.mBones[nodeIndex];
                    bone.mName      = node.mName;
                    bone.mNumChilds = (int)node.mChildren.size();
                    fanAssert( bone.mNumChilds < Bone::sMaxChilds );
                    for( int childIndex = 0; childIndex < node.mChildren.size(); ++childIndex )
                    {
                        bone.mChilds[childIndex] = remapTable.at( node.mChildren[childIndex] );
                    }
                    bone.mTransform = Matrix4( node.mRotation, node.mPosition, node.mScale );
                }

                Matrix4 skeletonTransform = Matrix4::sIdentity;
                GLTFNode& skeletonRoot = nodes[skin.mJoints[0]];
                if( skeletonRoot.mParent >= 0 )
                {
                    GLTFNode* node = &nodes[skeletonRoot.mParent];
                    while( node != nullptr )
                    {
                        Matrix4 relativeTransform( node->mRotation, node->mPosition, node->mScale );
                        skeletonTransform = relativeTransform * skeletonTransform;
                        node              = node->mParent >= 0 ? &nodes[node->mParent] : nullptr;
                    }
                }
                Bone& rootBone = submesh.mSkeleton.mBones[0];
                rootBone.mTransform = skeletonTransform * rootBone.mTransform ;

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
                meshVertices[i].mPos         = submesh.positionsArray[i];
                meshVertices[i].mNormal      = submesh.normalsArray != nullptr ? submesh.normalsArray[i] : glm::vec3( 0, 0, 1 );
                meshVertices[i].mColor       = Color::sWhite.ToGLM3();
                meshVertices[i].mUv          = submesh.texcoords0Array != nullptr ? submesh.texcoords0Array[i] : glm::vec2( 0, 0 );
                meshVertices[i].mBoneIDs     = submesh.joints0Array != nullptr ? submesh.joints0Array[i] : glm::u8vec4( 0, 0, 0, 42 );
                meshVertices[i].mBoneWeights = submesh.weights0Array != nullptr ? submesh.weights0Array[i] : glm::vec4( 1, 0, 0, 42 );
            }

            _mesh.mSkeleton = submesh.mSkeleton;
        }
    }
}
