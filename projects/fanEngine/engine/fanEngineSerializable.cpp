#include "engine/fanEngineSerializable.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanSceneResourcePtr.hpp"
#include "engine/singletons/fanScenePointers.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveComponentPtr( Json& _json, const char* _name, const ComponentPtrBase& _ptr )
    {
        _json[_name]["handle"]         = _ptr.mHandle;
        _json[_name]["component_type"] = _ptr.mType;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadComponentPtr( const Json& _json, const char* _name, ComponentPtrBase& _outPtr )
    {
        const Json* token = FindToken( _json, _name );
        if( token == nullptr ){ return false; }

        uint32_t componentType = ( *token )["component_type"];
        if( _outPtr.mType != componentType ){ return false; }

        _outPtr.CreateUnresolved( ( *token )["handle"] );
        return true;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveSlotPtr( Json& _json, const char* _name, const SlotPtr& _ptr )
    {
        const SlotPtr::SlotCallData& data = _ptr.Data();
        _json[_name]["handle"]    = data.mHandle;
        _json[_name]["type"]      = data.mType;
        _json[_name]["args_type"] = _ptr.GetArgsType();
        _json[_name]["slot"]      = ( data.mSlot != nullptr ? data.mSlot->mName : "" );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadSlotPtr( const Json& _json, const char* _name, SlotPtr& _outPtr )
    {
        const Json* token = FindToken( _json, _name );
        if( token == nullptr ){ return false; }

        EcsHandle handle = ( *token )["handle"];
        uint32_t  type   = ( *token )["type"];

        std::string slotName = "";
        int         argsType = ( *token )["args_type"];
        if( argsType == _outPtr.GetArgsType() )
        {
            slotName = ( *token )["slot"];
            if( handle != 0 )
            {
                _outPtr.SetComponentSlot( handle, type, slotName );
            }
            else
            {
                _outPtr.SetSingletonSlot( type, slotName );
            }
        }

        if( handle != 0 )
        {
            ScenePointers& scenePointers = _outPtr.World().GetSingleton<ScenePointers>();
            scenePointers.mUnresolvedSlotPtr.push_back( &_outPtr );
        }

        return true;
    }
}