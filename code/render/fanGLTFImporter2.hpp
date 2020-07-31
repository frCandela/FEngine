#pragma once

#include "fanJson.hpp"
#include <string>


namespace fan
{

	class Mesh2;

	//================================================================================================================================
	// For importing GLTF file
	// glTF� is a specification for the efficient transmission and loading of 3D scenes and models by applications.
	// It consist in a json header describing the data and binary (or base64) buffers
	// Some nested structs are defined below to help extracting data from the json.
	// see https://github.com/KhronosGroup/glTF
	//================================================================================================================================
	class GLTFImporter2
	{
	public:
		bool Load( const std::string _relativePath );
		bool GetMesh( Mesh2& _mesh );

	private:
		enum ComponentTypes
		{
			FLOAT = 5126, SCALAR = 5123
		};

		//================================================================		
		// A mesh primitive has a rendering mode (point, line or triangle) and 
		// series of attributes like positions or normals referencing accessors
		//================================================================
		struct GLTFPrimitive
		{
			GLTFPrimitive( const Json& jPrimitive );
			int	indices = -1;
			int	positions = -1;
			int	normal = -1;
			int	texcoord0 = -1;
			bool HasNormals() const { return normal >= 0; }
			bool HasTexcoords0() const { return texcoord0 >= 0; }
		};

		//================================================================
		// An accessor defines the type and layout of the data and references a buffer view
		//================================================================
		struct GLTFAccessor
		{
			GLTFAccessor( const Json& _jAccessor );
			int view = -1;
			int	type = -1;
			int	count = -1;
			std::string typeStr;
		};

		//================================================================
		// A  buffer view allows access to a  buffer 
		// it defines the part of the buffer that belongs to a buffer view
		//================================================================
		struct GLTFBufferView
		{
			GLTFBufferView( const Json& _jView );
			int buffer = -1;
			int	byteLength = -1;
			int	byteOffset = -1;
		};

		//================================================================
		// Contains data
		// it can be binary data stored in another file or base64 encoded data embeded in the json
		//================================================================
		struct GLTFBuffer
		{
			GLTFBuffer( const Json& _jBuffer );

			int byteLength = -1;

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

		std::string m_path;	// file relative path
		Json m_json;		// gltf json data

		std::string GLTFImporter2::DecodeBuffer( const std::string& _uri );
	};
}