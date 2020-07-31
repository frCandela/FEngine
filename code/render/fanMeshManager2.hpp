#pragma once

#include <string>
#include <vector>
#include "core/resources/fanResourcePtr.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
    class Mesh2;

    //================================================================================================================================
    //================================================================================================================================
    class MeshManager2
    {
    public:
        ~MeshManager2();

        Mesh2* GetMesh( const std::string& _path ) const;
        Mesh2* LoadMesh( const std::string& _path );
        void   AddMesh( Mesh2* mesh, const std::string& _name );
        void   Remove( const std::string& _path );
        void   Clear( Device& _device);
        bool   Empty() const { return mMeshes.empty(); }
        void   ResolvePtr( ResourcePtr<Mesh2>& _resourcePtr );

        void GenerateBuffers( Device& _device );
        void DestroyBuffers( Device& _device );

        int CountBuffersPendingDestruction() { return (int)mDestroyList.size(); }

    private:
        struct MeshData
        {
            Mesh2 *  mMesh = nullptr;
            Buffer   mIndexBuffer [ SwapChain::s_maxFramesInFlight ];
            Buffer   mVertexBuffer[ SwapChain::s_maxFramesInFlight ];
            uint32_t mCurrentBuffer = 0;
        };

        void GenerateGpuBuffers( Device& _device, MeshData& _meshData );

        std::vector< MeshData > mMeshes;
        std::vector< Buffer >   mDestroyList;
    };
}