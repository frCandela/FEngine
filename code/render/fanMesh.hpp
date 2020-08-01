#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResource.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMeshManager.hpp"
#include "render/core/fanBuffer.hpp"
#include "core/resources/fanResourcePtr.hpp"

struct Device;

namespace fan
{
    //================================================================================================================================
    // 3D mesh composed of triangles
    // can have a convex hull computed for it
    //================================================================================================================================
    class Mesh : public Resource
    {
    public:
        bool RayCast( const btVector3 _origin, const btVector3 _dir, btVector3& _outIntersection ) const;
        bool LoadFromFile( const std::string& _path );
        bool LoadFromVertices( const std::vector<Vertex>& _vertices );
        void OptimizeVertices();
        void GenerateConvexHull();
        void CreateBuffers( Device& _device );
        void DestroyBuffers( Device & _device );

        std::string             mPath;
        std::vector<Vertex>     mVertices;
        std::vector<uint32_t>   mIndices;
        ConvexHull              mConvexHull;
        Buffer                  mIndexBuffer [ SwapChain::s_maxFramesInFlight ];
        Buffer                  mVertexBuffer[ SwapChain::s_maxFramesInFlight ];
        uint32_t                mCurrentBuffer = 0;
        int                     mIndex = -1;
        bool                    mHostVisible      = false;
        bool                    mOptimizeVertices = true;
        bool                    mAutoUpdateHull   = true;
        bool                    mBuffersOutdated  = false;
        bool                    mExternallyOwned  = false;
    };
}