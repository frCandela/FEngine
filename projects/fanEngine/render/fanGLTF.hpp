#pragma once

#include "fanJson.hpp"
#include <string>

namespace fan
{
    // glTF is a specification for the efficient transmission and loading of 3D scenes and models.
    // It consist in a json header with binary (or base64) data buffers
    // source: https://github.com/KhronosGroup/glTF/tree/master/specification/2.0

    //================================================================
    //================================================================
    enum GLTFComponentType
    {
        Byte          = 5120, // bytes=1
        UnsignedByte  = 5121, // bytes=1
        Short         = 5122, // bytes=2
        UnsignedShort = 5123, // bytes=2
        UnsignedInt   = 5125, // bytes=4
        Float         = 5126, // bytes=4
    };

    //================================================================
    //================================================================
    enum GLTFType
    {
        Scalar = 0,
        Vec2,
        Vec3,
        Vec4,
        Mat2,
        Mat3,
        Mat4,
        Count,
    };

    static const char* sGLTFTypeStr[GLTFType::Count] =
                             {
                                     "SCALAR",// num components = 1
                                     "VEC2",  // num components = 2
                                     "VEC3",  // num components = 3
                                     "VEC4",  // num components = 4
                                     "MAT2",  // num components = 4
                                     "MAT3",  // num components = 9
                                     "MAT4",  // num components = 16
                             };

    //================================================================
    // A mesh primitive has a rendering mode (point, line or triangle) and
    // series of attributes like positions or normals referencing accessors
    //================================================================
    struct GLTFPrimitive
    {
        int mIndices   = -1;
        int mPosition  = -1;
        int mNormal    = -1;
        int mTexCoord0 = -1;
        int mMaterial  = 0;

        void Load( const Json& jPrimitive );
        void Save( Json& jPrimitive ) const;
        bool HasNormals() const { return mNormal >= 0; }
        bool HasTexcoords0() const { return mTexCoord0 >= 0; }
    };

    //================================================================
    // An accessor defines the type and layout of the data and references a buffer view
    //================================================================
    struct GLTFAccessor
    {
        int      mBufferView    = -1;
        int      mComponentType = -1;
        int      mCount         = -1;
        GLTFType mType          = GLTFType::Count;

        void Load( const Json& _jAccessor );
        void Save( Json& _jAccessor ) const;
    };

    //================================================================
    // A  buffer view allows access to a  buffer
    // it defines the part of the buffer that belongs to a buffer view
    //================================================================
    struct GLTFBufferView
    {
        int mBuffer     = -1;
        int mByteLength = -1;
        int mByteOffset = -1;
        //int mByteStride = -1;

        void Load( const Json& _jView );
        void Save( Json& _jBufferView ) const;
    };

    //================================================================
    // Contains data
    // it can be binary data stored in another file or base64 encoded data embedded in the json
    //================================================================
    struct GLTFBuffer
    {
        int mByteLength = -1;

        void Load( const Json& _jBuffer );
        void Save( Json& _jBuffer, const std::string& _uri ) const;
        std::string GetBuffer( const GLTFBufferView& _view, const std::string& _decodedBuffer ) const;
        static std::string DecodeBuffer( const std::string& _uri );
    };

    //================================================================
    // A mesh is a list of primitives
    // For now we only support one primitive per mesh
    //================================================================
    struct GLTFMesh
    {
        std::string                mName;
        std::vector<GLTFPrimitive> mPrimitives;

        void Load( const Json& jMesh );
        void Save( Json& jMesh ) const;
    };
}