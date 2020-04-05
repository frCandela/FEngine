#pragma once

#include "render/fanRenderPrecompiled.hpp"

#include "core/fanSerializable.hpp"

namespace fan
{
	//================================================================================================================================
	// utility functions for serializing renderer classes to json
	//================================================================================================================================
	class MeshPtr;
	class TexturePtr;

	namespace Serializable
	{
		void SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr );
		void SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr );

		bool LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr );
		bool LoadMeshPtr( const Json& _json, const char* _name, MeshPtr& _outPtr );
	}
}