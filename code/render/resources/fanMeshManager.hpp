#pragma once

#include <string>
#include <vector>
#include "core/resources/fanResourcePtr.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
    class Mesh;

    //========================================================================================================
    //========================================================================================================
    class MeshManager
    {
    public:
        Mesh*  Get( const std::string& _path ) const;
        Mesh*  Load( const std::string& _path );
        Mesh*  GetOrLoad( const std::string& _path );
        void   Add( Mesh* _mesh, const std::string& _name );
        void   Remove( const std::string& _path );
        void   Clear( Device& _device);
        bool   Empty() const { return mMeshes.empty(); }
        void   ResolvePtr( ResourcePtr<Mesh>& _resourcePtr );

        void Create( Device& _device );
        void Destroy( Device& _device );

        int DestroyListSize() const  { return (int)mDestroyList.size(); }
        const std::vector< Mesh * >& GetMeshes() const { return mMeshes; }
    private:
        std::vector< Mesh * > mMeshes;
        std::vector< Mesh * > mDestroyList;
    };
}