#include "scene/fanSceneSerializable.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/singletons/fanScenePointers.hpp"

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
		if (token == nullptr ) { return false; }

        uint32_t    componentType = ( *token )["component_type"];
		if(_outPtr.type != componentType ) { return false; }

		_outPtr.CreateUnresolved( (*token)["handle"] );
		return true;
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

    //========================================================================================================
    //========================================================================================================
    void Serializable::SaveSlotPtr( Json& _json, const char* _name, const SlotPtr& _ptr     )
    {
        const SlotPtr::SlotCallData& data = _ptr.Data();
        _json[_name]["handle"] = data.mHandle;
        _json[_name]["component_type"] = data.mComponentType;
        _json[_name]["args_type"] =  ( data.mSlot != nullptr ? data.mSlot->GetType() : -1 );
        _json[_name]["slot"] = ( data.mSlot != nullptr ?  data.mSlot->mName : "" );
    }

    //========================================================================================================
    //========================================================================================================
    bool Serializable::LoadSlotPtr( const Json& _json, const char* _name, SlotPtr& _outPtr )
    {
        const Json* token = FindToken( _json, _name );
        if( token == nullptr ){ return false; }

        EcsHandle   handle        = ( *token )["handle"];
        uint32_t    componentType = ( *token )["component_type"];
        std::string slotName = "";

        int argsType = ( *token )["args_type"];
        if( argsType == _outPtr.GetType() )
        {
            slotName= ( *token )["slot"];
        }
        _outPtr.Set( handle, componentType, slotName );

        if( handle != 0 )
        {
            ScenePointers& scenePointers = _outPtr.World().GetSingleton<ScenePointers>();
            scenePointers.mUnresolvedSlotPtr.push_back( &_outPtr );
        }

        return true;
    }
}