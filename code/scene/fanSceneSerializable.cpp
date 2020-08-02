#include "scene/fanSceneSerializable.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Serializable::SaveComponentPtr(Json& _json, const char* _name, const ComponentPtrBase& _ptr)
	{
		_json[_name]["handle"] = _ptr.handle;
		_json[_name]["component_type"] = _ptr.type;
	}

	//========================================================================================================
	//========================================================================================================
	void  Serializable::SavePrefabPtr( Json& _json, const char* _name, const PrefabPtr& _ptr )
	{
        _json[_name] = ( *_ptr != nullptr ? _ptr->mPath : "" );
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadComponentPtr( const Json& _json, const char* _name, ComponentPtrBase& _outPtr )
	{
		const Json* token = FindToken(_json, _name);
		if (token != nullptr)
		{
			assert( _outPtr.type == ( *token )["component_type"] );
			_outPtr.CreateUnresolved( (*token)["handle"] );
			return true;
		}
		return false;
	}

	//========================================================================================================
	//========================================================================================================
	bool Serializable::LoadPrefabPtr( const Json& _json, const char* _name, PrefabPtr& _outPtr )
	{
		const Json* token = FindToken( _json, _name );
		if ( token != nullptr )
		{
			_outPtr.Init(*token);
			_outPtr.Resolve();
			return true;
		}
		return false;
	}
}