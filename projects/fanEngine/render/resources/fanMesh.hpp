#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResource.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"
#include "core/resources/fanResourcePtr.hpp"

struct Device;

namespace fan
{
    //==================================================================================================================================================================================================
    // 3D mesh composed of triangles
    // can have a convex hull computed for it
    //==================================================================================================================================================================================================
    struct Mesh : public Resource
    {
        bool RayCast( const Vector3 _origin, const Vector3 _dir, Vector3& _outIntersection ) const;
        bool LoadFromFile( const std::string& _path );
        bool LoadFromVertices();
        void OptimizeVertices();
        void GenerateConvexHull();
        void Create( Device& _device );
        void Destroy( Device& _device );

        std::string           mPath;
        std::vector<Vertex>   mVertices;
        std::vector<uint32_t> mIndices;
        ConvexHull            mConvexHull;
        Buffer                mIndexBuffer[SwapChain::sMaxFramesInFlight];
        Buffer                mVertexBuffer[SwapChain::sMaxFramesInFlight];
        uint32_t              mCurrentBuffer    = 0;
        int                   mIndex            = -1;
        bool                  mHostVisible      = false;
        bool                  mOptimizeVertices = true;
        bool                  mAutoUpdateHull   = true;
        bool                  mBuffersOutdated  = false;
    };
}