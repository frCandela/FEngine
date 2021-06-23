#include "engine/fanEngineSerializable.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "engine/resources/fanComponentPtr.hpp"
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
}