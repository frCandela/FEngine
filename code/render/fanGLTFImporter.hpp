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
		GLTFImporter();
		~GLTFImporter();
		bool Load( const std::string _relativePath );
		bool GetMesh( Mesh  & _mesh );					

	private:
		std::string m_path;	// file relative path
		Json m_json;		// gltf json data
	};
}
