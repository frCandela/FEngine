#pragma once

#include "core/fanSerializable.hpp"

namespace fan
{
	//========================================================================================================
	// utility functions for serializing renderer classes to json
	//========================================================================================================
	class MeshPtr;
	class TexturePtr;
    class FontPtr;

	namespace Serializable
	{
		void SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr );
		void SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr );
        void SaveFontPtr( Json& _json, const char* _name, const FontPtr& _ptr );

		bool LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr );
		bool LoadMeshPtr( const Json& _json, const char* _name, MeshPtr& _outPtr );
        bool LoadFontPtr( const Json& _json, const char* _name, FontPtr& _outPtr);
	}
}