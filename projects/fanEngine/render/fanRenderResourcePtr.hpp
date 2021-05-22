#pragma once

#include "core/resources/fanResourcePtr.hpp"
#include "render/resources/fanTexture.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/resources/fanFont.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class TexturePtr : public ResourcePtr<Texture>
    {
    public:
        TexturePtr( Texture* _texture = nullptr ) : ResourcePtr<Texture>( _texture ) {}

        void Init( const std::string _path ) { mPath = _path; }
        const std::string& GetPath() const { return mPath; }

        ResourcePtr& operator=( Texture* _resource )
        {
            mData.mResource = _resource ;
            return *this;
        }
    private:
        std::string mPath;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class MeshPtr : public ResourcePtr<Mesh>
    {
    public:
        MeshPtr( Mesh* _mesh = nullptr ) : ResourcePtr<Mesh>( _mesh ) {}

        void Init( const std::string _path ) { mPath = _path; }
        const std::string& GetPath() const { return mPath; }

        ResourcePtr& operator=( Mesh* _resource )
        {
            mData.mResource =  _resource;
            return *this;
        }
    private:
        std::string mPath;
    };
}
