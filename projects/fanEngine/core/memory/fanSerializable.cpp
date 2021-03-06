#include "core/memory/fanSerializable.hpp"
#include "core/fanDebug.hpp"
#include "core/fanColor.hpp"
#include "core/fanPath.hpp"
#include "core/math/fanVector3.hpp"
#include "core/math/fanQuaternion.hpp"
#include "core/resources/fanResources.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveIVec2( Json& _json, const char* _name, const glm::ivec2& _vec2 )
    {
        _json[_name]["x"] = _vec2.x;
        _json[_name]["y"] = _vec2.y;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveVec2( Json& _json, const char* _name, const glm::vec2& _vec2 )
    {
        _json[_name]["x"] = _vec2[0];
        _json[_name]["y"] = _vec2[1];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveVec3( Json& _json, const char* _name, const glm::vec3& _vec3 )
    {
        _json[_name]["x"] = _vec3.x;
        _json[_name]["y"] = _vec3.y;
        _json[_name]["z"] = _vec3.z;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveVec3( Json& _json, const char* _name, const Vector3& _vec3 )
    {
        _json[_name]["x"] = _vec3.x.GetData();
        _json[_name]["y"] = _vec3.y.GetData();
        _json[_name]["z"] = _vec3.z.GetData();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveQuat( Json& _json, const char* _name, const Quaternion& _quat )
    {
        _json[_name]["n"] = _quat.mAngle.GetData();
        _json[_name]["x"] = _quat.mAxis.x.GetData();
        _json[_name]["y"] = _quat.mAxis.y.GetData();
        _json[_name]["z"] = _quat.mAxis.z.GetData();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveColor( Json& _json, const char* _name, const Color& _color )
    {
        _json[_name]["r"] = _color[0];
        _json[_name]["g"] = _color[1];
        _json[_name]["b"] = _color[2];
        _json[_name]["a"] = _color[3];
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveFloat( Json& _json, const char* _name, const float& _float )
    {
        _json[_name] = _float;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveFixed( Json& _json, const char* _name, const Fixed& _fixed )
    {
        _json[_name] = _fixed.GetData();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveInt( Json& _json, const char* _name, const int& _int )
    {
        _json[_name] = _int;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveInt2( Json& _json, const char* _name, const glm::ivec2& _int2 )
    {
        _json[_name][0] = _int2.x;
        _json[_name][1] = _int2.y;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveInt3( Json& _json, const char* _name, const glm::ivec3& _int3 )
    {
        _json[_name]["x"] = _int3.x;
        _json[_name]["y"] = _int3.y;
        _json[_name]["z"] = _int3.z;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveUInt( Json& _json, const char* _name, const unsigned& _int )
    {
        _json[_name] = _int;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveUInt64( Json& _json, const char* _name, const uint64_t& _uint64 )
    {
        _json[_name] = _uint64;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveBool( Json& _json, const char* _name, const bool& _bool )
    {
        _json[_name] = _bool;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveStr( Json& _json, const char* _name, const std::string& _string )
    {
        _json[_name] = _string.c_str();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    // LOAD
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadIVec2( const Json& _json, const char* _name, glm::ivec2& _outVec2 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outVec2.x = ( *token )["x"];
            _outVec2.y = ( *token )["y"];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadVec2( const Json& _json, const char* _name, glm::vec2& _outVec2 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outVec2[0] = ( *token )["x"];
            _outVec2[1] = ( *token )["y"];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadVec3( const Json& _json, const char* _name, glm::vec3& _outVec3 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outVec3.x = ( *token )["x"];
            _outVec3.y = ( *token )["y"];
            _outVec3.z = ( *token )["z"];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadVec3( const Json& _json, const char* _name, Vector3& _outVec3 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outVec3.x.SetData( ( *token )["x"] );
            _outVec3.y.SetData( ( *token )["y"] );
            _outVec3.z.SetData( ( *token )["z"] );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadQuat( const Json& _json, const char* _name, Quaternion& _outQuat )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outQuat.mAngle.SetData( ( *token )["n"] );
            _outQuat.mAxis.x.SetData( ( *token )["x"] );
            _outQuat.mAxis.y.SetData( ( *token )["y"] );
            _outQuat.mAxis.z.SetData( ( *token )["z"] );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadColor( const Json& _json, const char* _name, Color& _outColor )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outColor[0] = ( *token )["r"];
            _outColor[1] = ( *token )["g"];
            _outColor[2] = ( *token )["b"];
            _outColor[3] = ( *token )["a"];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadFloat( const Json& _json, const char* _name, float& _outFloat )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outFloat = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadFixed( const Json& _json, const char* _name, Fixed& _outFixed )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outFixed.SetData( static_cast<Fixed::DataType>(*token ) );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadInt( const Json& _json, const char* _name, int& _outInt )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outInt = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadInt2( const Json& _json, const char* _name, glm::ivec2& _outInt2 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outInt2.x = ( *token )[0];
            _outInt2.y = ( *token )[1];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadInt3( const Json& _json, const char* _name, glm::ivec3& _outInt3 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outInt3.x = ( *token )["x"];
            _outInt3.y = ( *token )["y"];
            _outInt3.z = ( *token )["z"];
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadUInt( const Json& _json, const char* _name, unsigned& _outUInt )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outUInt = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadUInt64( const Json& _json, const char* _name, uint64_t& _outUInt64 )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outUInt64 = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadBool( const Json& _json, const char* _name, bool& _outBool )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outBool = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadStr( const Json& _json, const char* _name, std::string& _outString )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            _outString = ( *token );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveResourcePtr( Json& _json, const char* _name, const ResourcePtrData& _ptrData )
    {
        bool resourceExists = _ptrData.mHandle != nullptr && _ptrData.mHandle->mResource != nullptr;
        _json[_name]["resource"] = resourceExists ? _ptrData.mHandle->mResource->mGUID : 0;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadResourcePtr( const Json& _json, const char* _name, ResourcePtrData& _outPtrData )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            const uint32_t guid = ( *token )["resource"];
            _outPtrData = ResourcePtrData::sResourceManager->Get( guid );
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    // returns true if a token exists
    //==================================================================================================================================================================================================
    bool Serializable::ContainsToken( const Json& _json, const char* _name )
    {
        return _json.find( _name ) != _json.end();
    }

    //==================================================================================================================================================================================================
    // returns the json token associated with a name or nullptr if it doesn't exists
    //==================================================================================================================================================================================================
    const Json* Serializable::FindToken( const Json& _json, const char* _name )
    {
        auto it = _json.find( _name );
        if( it != _json.end() )
        {
            return &( *it );
        }
        Debug::Warning() << "Failed to find token: " << _name << Debug::Endl();
        return nullptr;
    }
}