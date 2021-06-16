#include <core/fanDebug.hpp>
#include "fanGLTF.hpp"
#include "core/memory/fanBase64.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFPrimitive::Load( const Json& jPrimitive )
    {
        const Json& jAttribute = jPrimitive["attributes"];
        const bool hasMaterial  = jPrimitive.find( "material" ) != jPrimitive.end();
        const bool hasNormal    = jAttribute.find( "NORMAL" ) != jAttribute.end();
        const bool hasTexcoord0 = jAttribute.find( "TEXCOORD_0" ) != jAttribute.end();
        const bool hasJoints0   = jAttribute.find( "JOINTS_0" ) != jAttribute.end();
        const bool hasWeights0  = jAttribute.find( "WEIGHTS_0" ) != jAttribute.end();

        mIndices   = jPrimitive["indices"];
        mPosition  = jAttribute["POSITION"];
        mMaterial  = hasMaterial ? int( jPrimitive["material"] ) : 0;
        mNormal    = hasNormal ? int( jAttribute["NORMAL"] ) : -1;
        mTexCoord0 = hasTexcoord0 ? int( jAttribute["TEXCOORD_0"] ) : -1;
        mJoints0   = hasJoints0 ? int( jAttribute["JOINTS_0"] ) : -1;
        mWeights0  = hasWeights0 ? int( jAttribute["WEIGHTS_0"] ) : -1;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFPrimitive::Save( Json& jPrimitive ) const
    {
        jPrimitive["material"] = mMaterial;
        if( mIndices >= 0 ){ jPrimitive["indices"] = mIndices; }
        Json& jAttribute = jPrimitive["attributes"];
        {
            if( mPosition >= 0 ){ jAttribute["POSITION"] = mPosition; }
            if( mNormal >= 0 ){ jAttribute["NORMAL"] = mNormal; }
            if( mTexCoord0 >= 0 ){ jAttribute["TEXCOORD_0"] = mTexCoord0; }
            if( mJoints0 >= 0 ){ jAttribute["JOINTS_0"] = mJoints0; }
            if( mWeights0 >= 0 ){ jAttribute["WEIGHTS_0"] = mWeights0; }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFMesh::Load( const Json& jMesh )
    {
        mName = jMesh["name"];
        const Json& jPrimitives = jMesh["primitives"];
        mPrimitives.resize( jPrimitives.size() );
        for( int i = 0; i < jPrimitives.size(); ++i )
        {
            mPrimitives[i].Load( jPrimitives[i] );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFMesh::Save( Json& jMesh ) const
    {
        jMesh["name"] = mName;
        for( int i = 0; i < mPrimitives.size(); ++i )
        {
            mPrimitives[i].Save( jMesh["primitives"][i] );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFBufferView::Load( const Json& _jView )
    {
        fanAssert( _jView.find( "byteStride" ) == _jView.end() );
        mBuffer     = _jView["buffer"];
        mByteLength = _jView["byteLength"];
        mByteOffset = _jView["byteOffset"];

        //const bool hasStride = _jView.find( "byteStride" ) != _jView.end();
        //mByteStride = hasStride ? int( _jView["byteStride"] ) : -1;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFBufferView::Save( Json& _jBufferView ) const
    {
        _jBufferView["buffer"]     = mBuffer;
        _jBufferView["byteLength"] = mByteLength;
        _jBufferView["byteOffset"] = mByteOffset;
        //if( mByteStride >= 0 ){ _jBufferView["byteStride"] = mByteStride; }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFAccessor::Load( const Json& _jAccessor )
    {
        mBufferView    = _jAccessor["bufferView"];
        mComponentType = _jAccessor["componentType"];
        mCount         = _jAccessor["count"];

        const std::string strType = _jAccessor["type"];
        for( int          i       = 0; i < (int)GLTFType::Count; ++i )
        {
            if( strType == sGLTFTypeStr[i] )
            {
                mType = GLTFType( i );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFAccessor::Save( Json& _jAccessor ) const
    {
        _jAccessor["bufferView"]    = mBufferView;
        _jAccessor["componentType"] = mComponentType;
        _jAccessor["count"]         = mCount;
        _jAccessor["type"]          = sGLTFTypeStr[(int)mType];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFBuffer::Load( const Json& _jBuffer )
    {
        mByteLength = _jBuffer["byteLength"];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFBuffer::Save( Json& _jBuffer, const std::string& _uri ) const
    {
        _jBuffer["byteLength"] = mByteLength;
        _jBuffer["uri"]        = _uri;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFSkin::Load( const Json& _jSkin )
    {
        mName                = _jSkin["name"];
        mInverseBindMatrices = _jSkin["inverseBindMatrices"];

        const Json& jJoints = _jSkin["joints"];
        fanAssert( jJoints.is_array() );
        mJoints.clear();
        for( int i = 0; i < jJoints.size(); ++i )
        {
            mJoints.push_back( jJoints[i] );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFSkin::Save( Json& _jSkin ) const
    {
        _jSkin["name"]                = mName;
        _jSkin["inverseBindMatrices"] = mInverseBindMatrices;
        Json& jJoints = _jSkin["joints"];
        for( int i = 0; i < mJoints.size(); ++i )
        {
            jJoints.push_back( mJoints[i] );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFNode::Load( const Json& _jNode )
    {
        mName = _jNode["name"];

        const auto itPosition = _jNode.find( "translation" );
        if( itPosition != _jNode.end() )
        {
            const Json& _jPosition = *itPosition;
            mPosition.x = Fixed::FromFloat( _jPosition[0] );
            mPosition.y = Fixed::FromFloat( _jPosition[1] );
            mPosition.z = Fixed::FromFloat( _jPosition[2] );
        }

        const auto itScale = _jNode.find( "scale" );
        if( itScale != _jNode.end() )
        {
            const Json& _jScale = *itScale;
            mScale.x = Fixed::FromFloat( _jScale[0] );
            mScale.y = Fixed::FromFloat( _jScale[1] );
            mScale.z = Fixed::FromFloat( _jScale[2] );
        }
        else
        {
            mScale = Vector3::sOne;
        }

        const auto itRotation = _jNode.find( "rotation" );
        if( itRotation != _jNode.end() )
        {
            const Json& _jRotation = *itRotation;
            mRotation.mAxis.x = Fixed::FromFloat( _jRotation[0] );
            mRotation.mAxis.y = Fixed::FromFloat( _jRotation[1] );
            mRotation.mAxis.z = Fixed::FromFloat( _jRotation[2] );
            mRotation.mAngle  = Fixed::FromFloat( _jRotation[3] );
        }

        const auto itChildren = _jNode.find( "children" );
        if( itChildren != _jNode.end() )
        {
            mChildren.resize( itChildren->size() );
            for( int i = 0; i < itChildren->size(); i++ )
            {
                mChildren[i] = int( ( *itChildren )[i] );
                fanAssert( mChildren[i] >= 0 );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFNode::Save( Json& _jNode ) const
    {
        _jNode["name"] = mName;

        Json& jPosition = _jNode["translation"];
        jPosition[0] = mPosition.x.ToFloat();
        jPosition[1] = mPosition.y.ToFloat();
        jPosition[2] = mPosition.z.ToFloat();

        Json& jScale = _jNode["scale"];
        jScale[0] = mScale.x.ToFloat();
        jScale[1] = mScale.y.ToFloat();
        jScale[2] = mScale.z.ToFloat();

        Json& jRotation = _jNode["rotation"];
        jRotation[0] = mRotation.mAxis.x.ToFloat();
        jRotation[1] = mRotation.mAxis.y.ToFloat();
        jRotation[2] = mRotation.mAxis.z.ToFloat();
        jRotation[3] = mRotation.mAngle.ToFloat();

        Json& jChildren = _jNode["children"];
        for( int i = 0; i < mChildren.size(); i++ )
        {
            jChildren[i] = mChildren[i] + 1;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFScene::Load( const Json& _jScene )
    {
        mName = _jScene["name"];
        const auto itNodes = _jScene.find( "nodes" );
        if( itNodes != _jScene.end() )
        {
            mNodes.resize( itNodes->size() );
            for( int i = 0; i < itNodes->size(); i++ )
            {
                mNodes[i] = int( ( *itNodes )[i] );
                fanAssert( mNodes[i] >= 0 );
            }
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFScene::Save( Json& _jScene ) const
    {
        _jScene["name"] = mName;
        Json& jNodes = _jScene["nodes"];
        for( int i = 0; i < mNodes.size(); i++ )
        {
            jNodes[i] = mNodes[i] + 1;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFAnimation::Load( const Json& _jAnimation )
    {
        mName = _jAnimation["name"];

        // channels
        const Json& jChannels = _jAnimation["channels"];
        mChannels.resize( jChannels.size() );
        for( int i = 0; i < mChannels.size(); i++ )
        {
            const Json& jChannel = jChannels[i];
            Channel   & channel  = mChannels[i];
            channel.mSampler    = jChannel["sampler"];
            channel.mTargetNode = jChannel["target"]["node"];

            String<16> targetPathStr = jChannel["target"]["path"];
            if( targetPathStr == "translation" ){ channel.mTargetPath = GLTFAnimationPath::Translation; }
            else if( targetPathStr == "rotation" ){ channel.mTargetPath = GLTFAnimationPath::Rotation; }
            else if( targetPathStr == "scale" ){ channel.mTargetPath = GLTFAnimationPath::Scale; }
            else{ channel.mTargetPath = GLTFAnimationPath::Weights; }
        }

        // samplers
        const Json& jSamplers = _jAnimation["samplers"];
        mSamplers.resize( jSamplers.size() );
        for( int i = 0; i < mSamplers.size(); i++ )
        {
            const Json& jSampler = jSamplers[i];
            Sampler   & sampler  = mSamplers[i];
            sampler.mInput         = jSampler["input"];
            sampler.mInterpolation = jSampler["interpolation"];
            sampler.mOutput        = jSampler["output"];
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFAnimation::Save( Json& _jAnimation ) const
    {
        (void)_jAnimation;
    }

    //==================================================================================================================================================================================================
    // Decodes a buffer from base64 to binary
    // there is some header data at the beginnig of the buffer delimited with a ','
    //==================================================================================================================================================================================================
    std::string GLTFBuffer::DecodeBuffer( const std::string& _uri )
    {
        const std::string::const_iterator fullBufferBegin = _uri.begin() + _uri.find_first_of( ',' ) + 1;
        return Base64::Decode( std::string( fullBufferBegin, _uri.end() ) );
    }

    //==================================================================================================================================================================================================
    // Extracts and returns a segment of buffer from a view
    //==================================================================================================================================================================================================
    std::string GLTFBuffer::GetBuffer( const GLTFBufferView& _view, const std::string& _decodedBuffer ) const
    {
        return std::string( _decodedBuffer.begin() + _view.mByteOffset, _decodedBuffer.begin() + _view.mByteOffset + _view.mByteLength );
    }
}
