#pragma once

#include "core/resources/fanResource.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"

namespace fan
{
    struct Device;

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Mesh2D : public Resource
    {
    FAN_RESOURCE( Mesh2D );

        bool LoadFromVertices( const std::vector<UIVertex>& _vertices );
        void Create( Device& _device );
        void Destroy( Device& _device );

        std::vector<UIVertex> mVertices;
        Buffer                mVertexBuffer;
        int                   mIndex           = -1;
        bool                  mHostVisible     = false;
    };
}