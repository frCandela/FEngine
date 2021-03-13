#pragma once

#include "fanJson.hpp"
#include <string>

namespace fan
{

	struct Mesh;

	//========================================================================================================
	// For importing GLTF file
	// glTF is a specification for the efficient transmission and loading of 3D scenes and models.
	// It consist in a json header describing the data and binary (or base64) buffers
	// Some nested structs are defined below to help extracting data from the json.
	// see https://github.com/KhronosGroup/glTF
	//========================================================================================================
	class GLTFImporter
	{
	public:
		bool Load( const std::string _relativePath );
		bool GetMesh( Mesh& _mesh );

	private:
		enum ComponentTypes
		{
            Float = 5126, Scalar = 5123
		};

		//================================================================		
		// A mesh primitive has a rendering mode (point, line or triangle) and 
		// series of attributes like positions or normals referencing accessors
		//================================================================
		struct GLTFPrimitive
		{
			GLTFPrimitive( const Json& jPrimitive );
			int mIndices   = -1;
			int mPositions = -1;
			int mNormal    = -1;
			int mTexcoord0 = -1;
			bool HasNormals() const { return mNormal >= 0; }
			bool HasTexcoords0() const { return mTexcoord0 >= 0; }
		};

		//================================================================
		// An accessor defines the type and layout of the data and references a buffer view
		//================================================================
		struct GLTFAccessor
		{
			GLTFAccessor( const Json& _jAccessor );
			int         mView  = -1;
			int         mType  = -1;
			int         mCount = -1;
			std::string mTypeStr;
		};

		//================================================================
		// A  buffer view allows access to a  buffer 
		// it defines the part of the buffer that belongs to a buffer view
		//================================================================
		struct GLTFBufferView
		{
			GLTFBufferView( const Json& _jView );
			int mBuffer     = -1;
			int mByteLength = -1;
			int mByteOffset = -1;
		};

		//================================================================
		// Contains data
		// it can be binary data stored in another file or base64 encoded data embeded in the json
		//================================================================
		struct GLTFBuffer
		{
			GLTFBuffer( const Json& _jBuffer );

			int mByteLength = -1;

			std::string GetBuffer( const GLTFBufferView& _view, const std::string& _decodedBuffer ) const;
		};

		//================================================================
		// A mesh is a list of primitives
		// For now we only support one primitive per mesh
		//================================================================
		struct GLTFMesh
		{
			GLTFMesh( const Json& jMesh );
			std::string name;
			GLTFPrimitive primitive0;
		};

		std::string mPath;	// file relative path
		Json        mJson;	// gltf json data

		std::string DecodeBuffer( const std::string& _uri );
	};
}