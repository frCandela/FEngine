#include "render/fanRenderSerializable.hpp"

#include "core/fanPath.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
    static_assert( ( std::is_base_of<Resource, Mesh>::value ) );
    static_assert( ( std::is_base_of<Resource, Texture>::value ) );

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveTexturePtr( Json& _json, const char* _name, const TexturePtr& _ptr )
    {
        _json[_name] = *_ptr != nullptr ? Path::MakeRelative( _ptr->mPath ) : "";
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Serializable::SaveMeshPtr( Json& _json, const char* _name, const MeshPtr& _ptr )
    {
        _json[_name] = *_ptr != nullptr ? Path::MakeRelative( _ptr->mPath ) : "";
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadTexturePtr( const Json& _json, const char* _name, TexturePtr& _outPtr )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            fanAssert( !_outPtr.IsValid() );
            _outPtr.Init( *token );
            _outPtr.mData.Resolve();
            return true;
        }
        return false;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    bool Serializable::LoadMeshPtr( const Json& _json, const char* _name, MeshPtr& _outPtr )
    {
        const Json* token = FindToken( _json, _name );
        if( token != nullptr )
        {
            fanAssert( !_outPtr.IsValid() );
            _outPtr.Init( *token );
            _outPtr.mData.Resolve();
            return true;
        }
        return false;
    }
}