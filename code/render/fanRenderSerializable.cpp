#include "render/fanRenderSerializable.hpp"

#include "render/fanRenderResourcePtr.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	static_assert( ( std::is_base_of<Resource, Mesh>::value ) );
	static_assert( ( std::is_base_of<Resource, Texture>::value ) );

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->mPath : "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->GetPath() : "";
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			assert( !_outPtr.IsValid() );
			_outPtr.Init( *token );
			_outPtr.Resolve();
			return true;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadMeshPtr( const Json& _json, const char* _name, MeshPtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			assert( !_outPtr.IsValid() );
			_outPtr.Init( *token );
			_outPtr.Resolve();
			return true;
		}
		return false;
	}
}