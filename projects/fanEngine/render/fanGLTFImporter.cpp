#include <fstream>
#include "core/fanPath.hpp"
#include "core/fanDebug.hpp"
#include "render/fanGLTFImporter.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanSkinnedMesh.hpp"
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
        const Json& jAnimations  = mJson["animations"];

        // decode buffers
        std::vector<std::string> loadedBuffers;
        loadedBuffers.resize( jBuffers.size() );
        for( int i = 0; i < jBuffers.size(); ++i )
        {
            loadedBuffers[i] = GLTFBuffer::DecodeBuffer( jBuffers[i]["uri"] );
        }

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

        // loads skins
        std::map<int, int> bonesRemapTable; // node index -> bone index
        if( jSkins.is_array() && jSkins.size() != 0 )
        {
            GLTFSkin skin;
            skin.Load( jSkins[0] );
            mSkeleton.mName     = skin.mName;
            mSkeleton.mNumBones = (int)skin.mJoints.size();

            // load inverse bind matrices
            GLTFAccessor bindMatricesAccessor;
            bindMatricesAccessor.Load( jAccessors[skin.mInverseBindMatrices] );
            fanAssert( bindMatricesAccessor.mComponentType == GLTFComponentType::Float );
            fanAssert( bindMatricesAccessor.mType == GLTFType::Mat4 );
            fanAssert( bindMatricesAccessor.mCount == mSkeleton.mNumBones );
            GLTFBufferView viewBindMatrices;
            viewBindMatrices.Load( jBufferViews[bindMatricesAccessor.mBufferView] );
            fanAssert( viewBindMatrices.mByteLength == sizeof( glm::mat4 ) * bindMatricesAccessor.mCount );
            GLTFBuffer bufferBindMatrices;
            bufferBindMatrices.Load( jBuffers[viewBindMatrices.mBuffer] );
            std::string bindMatricesBuffer = bufferBindMatrices.GetBuffer( viewBindMatrices, loadedBuffers[viewBindMatrices.mBuffer] );
            glm::mat4* bindMatricesArray = (glm::mat4*)bindMatricesBuffer.data();

            // generate a remap table to generate the bones in a contiguous array
            std::vector<GLTFNode> skeletonNodes;
            skeletonNodes.resize( skin.mJoints.size());
            fanAssert( skin.mJoints.size() <= RenderGlobal::sMaxBones );
            for( int i = 0; i < skin.mJoints.size(); i++ )
            {
                const int nodeIndex = skin.mJoints[i];
                const GLTFNode& node = nodes[nodeIndex];
                bonesRemapTable[nodeIndex] = i;
                skeletonNodes[i]           = node;
            }

            // generate the bones
            for( int nodeIndex = 0; nodeIndex < skeletonNodes.size(); nodeIndex++ )
            {
                const GLTFNode& node = skeletonNodes[nodeIndex];
                Bone          & bone = mSkeleton.mBones[nodeIndex];
                bone.mName      = node.mName;
                bone.mNumChilds = (int)node.mChildren.size();
                fanAssert( bone.mNumChilds < Bone::sMaxChilds );
                for( int childIndex                     = 0; childIndex < node.mChildren.size(); ++childIndex )
                {
                    bone.mChilds[childIndex] = bonesRemapTable.at( node.mChildren[childIndex] );
                }
                mSkeleton.mInverseBindMatrix[nodeIndex] = Matrix4( bindMatricesArray[nodeIndex] );
            }

            // transforms root node to make it absolute
            GLTFNode * node = &nodes[skin.mJoints[0]];
            Matrix4 rootTransform = Matrix4::sIdentity;
            while(  node->mParent >= 0 )
            {
                node = &nodes[node->mParent];
                rootTransform = Matrix4( node->mRotation, node->mPosition, node->mScale ) * rootTransform;
            }
            mSkeleton.mRootTransform = rootTransform;

            // adds the skeleton root node...
            const int skeletonRootIndex = skeletonNodes[0].mParent;
            const GLTFNode& skeletonRoot = nodes[skeletonRootIndex];
            Bone          & rootBone =  mSkeleton.mBones[mSkeleton.mNumBones];
            bonesRemapTable[skeletonRootIndex] = mSkeleton.mNumBones;
            mSkeleton.mInverseBindMatrix[mSkeleton.mNumBones] = Matrix4::sIdentity;
            mSkeleton.mNumBones++;
            rootBone.mNumChilds = 1;
            rootBone.mChilds[0] = 0;
            rootBone.mName = skeletonRoot.mName;
        }

        // load animations
        mAnimations.resize( jAnimations.size() );
        for( int animIndex = 0; animIndex < jAnimations.size(); ++animIndex )
        {
            GLTFAnimation anim;
            anim.Load( jAnimations[animIndex] );

            Animation& animation = mAnimations[animIndex];
            animation.mNumBones = mSkeleton.mNumBones;
            fanAssert( mSkeleton.mNumBones > 0 );
            for( int channelIndex = 0; channelIndex < anim.mChannels.size(); ++channelIndex )
            {
                const GLTFAnimation::Channel& channel = anim.mChannels[channelIndex];
                const GLTFAnimation::Sampler& sampler = anim.mSamplers[channel.mSampler];
                GLTFAccessor accessorTime;
                accessorTime.Load( jAccessors[sampler.mInput] );
                fanAssert( accessorTime.mComponentType == GLTFComponentType::Float );
                fanAssert( accessorTime.mType == GLTFType::Scalar );
                GLTFBufferView viewTime;
                viewTime.Load( jBufferViews[accessorTime.mBufferView] );
                GLTFBuffer bufferTime;
                bufferTime.Load( jBuffers[viewTime.mBuffer] );

                std::string timeBufferStr = bufferTime.GetBuffer( viewTime, loadedBuffers[viewTime.mBuffer] );
                float* timeBuffer = (float*)timeBufferStr.data();

                GLTFAccessor accessorProperty;
                accessorProperty.Load( jAccessors[sampler.mOutput] );
                fanAssert( accessorProperty.mComponentType == GLTFComponentType::Float );
                GLTFBufferView viewProperty;
                viewProperty.Load( jBufferViews[accessorProperty.mBufferView] );
                fanAssert( accessorTime.mCount == accessorProperty.mCount );
                GLTFBuffer bufferProperty;
                bufferProperty.Load( jBuffers[viewProperty.mBuffer] );

                const int boneIndex = bonesRemapTable.at(channel.mTargetNode);
                Animation::BoneAnimation& boneAnimation = animation.mBoneKeys[boneIndex];
                switch( channel.mTargetPath )
                {
                    case GLTFAnimationPath::Translation:
                    {
                        fanAssert( accessorProperty.mType == GLTFType::Vec3 );
                        fanAssert( boneAnimation.mPositions.empty() );
                        std::string positionBufferStr = bufferProperty.GetBuffer( viewProperty, loadedBuffers[viewProperty.mBuffer] );
                        glm::vec3* positionBuffer = (glm::vec3*)positionBufferStr.data();
                        boneAnimation.mPositions.resize( accessorProperty.mCount );
                        for( int keyIndex = 0; keyIndex < accessorProperty.mCount; ++keyIndex )
                        {
                            boneAnimation.mPositions[keyIndex].mTime     = Fixed::FromFloat( timeBuffer[keyIndex] );
                            boneAnimation.mPositions[keyIndex].mPosition = Vector3( positionBuffer[keyIndex] );
                        }
                        break;
                    }
                    case GLTFAnimationPath::Rotation:
                    {
                        fanAssert( accessorProperty.mType == GLTFType::Vec4 );
                        fanAssert( boneAnimation.mRotations.empty() );
                        std::string rotationBufferStr = bufferProperty.GetBuffer( viewProperty, loadedBuffers[viewProperty.mBuffer] );
                        glm::quat* rotationBuffer = (glm::quat*)rotationBufferStr.data();
                        boneAnimation.mRotations.resize( accessorProperty.mCount );
                        for( int keyIndex = 0; keyIndex < accessorProperty.mCount; ++keyIndex )
                        {
                            boneAnimation.mRotations[keyIndex].mTime     = Fixed::FromFloat( timeBuffer[keyIndex] );
                            boneAnimation.mRotations[keyIndex].mRotation = Quaternion( rotationBuffer[keyIndex] );
                        }
                        break;
                    }
                    case GLTFAnimationPath::Scale:
                    {
                        fanAssert( accessorProperty.mType == GLTFType::Vec3 );
                        fanAssert( boneAnimation.mScales.empty() );
                        std::string scaleBufferStr = bufferProperty.GetBuffer( viewProperty, loadedBuffers[viewProperty.mBuffer] );
                        glm::vec3* scaleBuffer = (glm::vec3*)scaleBufferStr.data();
                        boneAnimation.mScales.resize( accessorProperty.mCount );
                        for( int keyIndex = 0; keyIndex < accessorProperty.mCount; ++keyIndex )
                        {
                            boneAnimation.mScales[keyIndex].mTime  = Fixed::FromFloat( timeBuffer[keyIndex] );
                            boneAnimation.mScales[keyIndex].mScale = Vector3( scaleBuffer[keyIndex] );
                        }
                        break;
                    }
                    default:
                        fanAssert( false );
                        break;
                }
            }

            // calculate animation duration
            for( int boneIndex = 0; boneIndex < animation.mNumBones; boneIndex++ )
            {
                Animation::BoneAnimation& boneAnimation = animation.mBoneKeys[boneIndex];
                Animation::KeyPosition& lastPosition = *boneAnimation.mPositions.rbegin();
                Animation::KeyScale& lastScale = *boneAnimation.mScales.rbegin();
                Animation::KeyRotation& lastRotation = *boneAnimation.mRotations.rbegin();
                animation.mDuration = Fixed::Max( animation.mDuration, lastPosition.mTime );
                animation.mDuration = Fixed::Max( animation.mDuration, lastScale.mTime );
                animation.mDuration = Fixed::Max( animation.mDuration, lastRotation.mTime );
            }
        }

        // Returns if there is no mesh
        if( jMeshes.is_null() || jMeshes.empty() )
        {
            Debug::Error() << "mesh is empty : " << mPath << Debug::Endl();
            return false;
        }

        // Loads the first mesh json
        GLTFMesh mesh;
        mesh.Load( jMeshes[0] );

        // Load submeshes vertices
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
                submesh.indexBuffer  = bufferIndices.GetBuffer( viewIndices, loadedBuffers[viewIndices.mBuffer] );
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
                submesh.posBuffer      = bufferPositions.GetBuffer( viewPositions, loadedBuffers[viewPositions.mBuffer] );
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
                submesh.normalsBuffer = bufferNormals.GetBuffer( viewNormals, loadedBuffers[viewNormals.mBuffer] );
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
                submesh.texcoords0Buffer = bufferTexcoord0.GetBuffer( viewTexcoords0, loadedBuffers[viewTexcoords0.mBuffer] );
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
                submesh.joints0Buffer = bufferJoints0.GetBuffer( viewJoints0, loadedBuffers[viewJoints0.mBuffer] );
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
                submesh.weights0Buffer = bufferWeights0.GetBuffer( viewWeight0, loadedBuffers[viewWeight0.mBuffer] );
                submesh.weights0Array  = (glm::vec4*)submesh.weights0Buffer.data();
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
        }
        _mesh.mSkeleton         = mSkeleton;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFImporter::GetAnimation( Animation& _animation )
    {
        if( !mAnimations.empty() )
        {
            _animation = mAnimations[0];
        }
    }
}
