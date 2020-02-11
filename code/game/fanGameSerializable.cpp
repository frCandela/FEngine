#include "game/fanGameSerializable.hpp"

#include "scene/components/fanComponent.hpp"
#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanComponentPtr.hpp"
#include "scene/fanPrefab.hpp"
#include "render/core/fanTexture.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	static_assert( ( std::is_base_of<Resource<Mesh>, Mesh>::value ) );
	static_assert( ( std::is_base_of<Resource<Texture>, Texture>::value ) );
	static_assert( ( std::is_base_of<Resource<Gameobject>, Gameobject>::value ) );
	static_assert( ( std::is_base_of<Resource<Component>, Component>::value ) );

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveGameobjectPtr( Json& _json, const char* _name, const GameobjectPtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->GetUniqueID() : 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->GetPath() : "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->GetPath() : "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void  Serializable::SavePrefabPtr( Json& _json, const char* _name, const PrefabPtr& _ptr )
	{
		_json[ _name ] = *_ptr != nullptr ? _ptr->GetPath() : "";
	}

	//================================================================================================================================
	//================================================================================================================================
	void Serializable::SaveComponentPtr( Json& _json, const char* _name, const ComponentIDPtr& _ptr )
	{
		_json[ _name ][ "gameobject_id" ] = *_ptr != nullptr ? _ptr->GetGameobject().GetUniqueID() : 0;
		_json[ _name ][ "component_id" ] = *_ptr != nullptr ? _ptr->GetType() : 0;
	}

	//================================================================================================================================
	//================================================================================================================================
	//LOAD
	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadGameobjectPtr( const Json& _json, const char* _name, GameobjectPtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			//_outPtr.Init(*token);@tmp
			return true;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			//_outPtr.Init(*token);@tmp
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
			//_outPtr.Init(*token);@tmp
			return true;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadPrefabPtr( const Json& _json, const char* _name, PrefabPtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			//_outPtr.Init(*token);@tmp
			return true;
		}
		return false;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Serializable::LoadComponentPtr( const Json& _json, const char* _name, ComponentIDPtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			//_outPtr.Init(IDPtrData((*token)["gameobject_id"], (*token)["component_id"]));@tmp
			return true;
		}
		return false;
	}
}