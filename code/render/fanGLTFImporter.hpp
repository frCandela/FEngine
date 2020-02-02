#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan {

	class Mesh;

	//================================================================================================================================
	// Allow import of GLTF file
	// glTF™ is a specification for the efficient transmission and loading of 3D scenes and models by applications.
	// see https://github.com/KhronosGroup/glTF
	//================================================================================================================================
	class GLTFImporter
	{
	public:
		bool Load( const std::string _relativePath );
		bool GetMesh( Mesh  & _mesh );	

	private:
		enum ComponentTypes 
		{
			FLOAT = 5126, SCALAR = 5123
		};

		//================================================================		
		//================================================================
		struct GLTFPrimitive {
			GLTFPrimitive(const Json& jPrimitive);
			int	indices = -1;
			int	positions = -1;
			int	normal   = -1;
			int	texcoord0 = -1;
			bool HasNormals() const { return normal >= 0; }
			bool HasTexcoords0() const { return texcoord0 >= 0; }
		};

		//================================================================
		// For now we only support one primitive per mesh
		//================================================================
		struct GLTFMesh {
			GLTFMesh(const Json& jMesh);
			std::string name;
			GLTFPrimitive primitive0;
		};

		//================================================================
		//================================================================
		struct GLTFBufferView {
			GLTFBufferView(const Json& _jView);
			int buffer = -1;
			int	byteLength = -1;
			int	byteOffset = -1;
		};

		//================================================================
		//================================================================
		struct GLTFAccessor 
		{
			GLTFAccessor(const Json& _jAccessor);
			int view = -1;
			int	type = -1;
			int	count = -1;
			std::string typeStr;
		};

		//================================================================
		//================================================================
		struct GLTFBuffer
		{
			GLTFBuffer(const Json& _jBuffer );

			int byteLength = -1;

			static std::string GetBuffer( const GLTFBufferView& _view, const std::string& _uri );
		};

		std::string m_path;	// file relative path
		Json m_json;		// gltf json data

		void GetBufferFromView( std::string& _outStringBuffer);

	};
}