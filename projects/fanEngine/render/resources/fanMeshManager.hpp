#pragma once

#include <string>
#include <vector>
#include "core/resources/fanResourcePtr.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
    struct Mesh;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class MeshManager
    {
    public:
        MeshManager() {};
        MeshManager( MeshManager const& ) = delete;
        MeshManager& operator=( MeshManager const& ) = delete;

        Mesh* Get( const std::string& _path ) const;
        Mesh* Load( const std::string& _path );
        Mesh* GetOrLoad( const std::string& _path );
        void Add( Mesh* _mesh, const std::string& _name );
        void Remove( const std::string& _path );
        void Clear( Device& _device );
        bool Empty() const { return mMeshes.empty(); }
        void ResolvePtr( ResourcePtrData& _resourcePtr );

        void CreateNewMeshes( Device& _device );
        void DestroyRemovedMeshes( Device& _device );

        int DestroyListSize() const { return (int)mDestroyList.size(); }
        const std::vector<Mesh*>& GetMeshes() const { return mMeshes; }
    private:

        std::vector<Mesh*> mMeshes;
        std::vector<Mesh*> mDestroyList;
    };
}