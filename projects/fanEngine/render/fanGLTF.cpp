#include "fanGLTF.hpp"
#include "core/memory/fanBase64.hpp"
#include "core/fanAssert.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void GLTFPrimitive::Load( const Json& jPrimitive )
    {
        const Json& jAttribute = jPrimitive["attributes"];
        const bool hasNormal    = jAttribute.find( "NORMAL" ) != jAttribute.end();
        const bool hasTexcoord0 = jAttribute.find( "TEXCOORD_0" ) != jAttribute.end();
        const bool hasMaterial  = jPrimitive.find( "material" ) != jPrimitive.end();

        mIndices   = jPrimitive["indices"];
        mPosition  = jAttribute["POSITION"];
        mMaterial  = hasMaterial ? int( jPrimitive["material"] ) : 0;
        mNormal    = hasNormal ? int( jAttribute["NORMAL"] ) : -1;
        mTexCoord0 = hasTexcoord0 ? int( jAttribute["TEXCOORD_0"] ) : -1;
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
        for( int          i       = 0; i < GLTFType::Count; ++i )
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
        _jAccessor["type"]          = sGLTFTypeStr[mType];
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
    // Decodes a buffer from base64 to binary
    // there is some header data at the beginnig of the buffer delimited with a ','
    //==================================================================================================================================================================================================
    std::string GLTFBuffer::DecodeBuffer( const std::string& _uri )
    {
        const std::string::const_iterator fullBufferBegin = _uri.begin() + _uri.find_first_of( ',' ) + 1;
        return Base64::Decode( std::string( fullBufferBegin, _uri.end() ) );
    }

    //==================================================================================================================================================================================================
    // Extracts, decodes and returns a segment of buffer from a view
    //==================================================================================================================================================================================================
    std::string GLTFBuffer::GetBuffer( const GLTFBufferView& _view, const std::string& _decodedBuffer ) const
    {
        return std::string( _decodedBuffer.begin() + _view.mByteOffset, _decodedBuffer.begin() + _view.mByteOffset + _view.mByteLength );
    }
}
