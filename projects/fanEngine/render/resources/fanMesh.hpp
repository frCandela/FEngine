#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "core/shapes/fanSphere.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"

struct Device;

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SubMesh
    {
        std::vector<Vertex>   mVertices;
        std::vector<uint32_t> mIndices;
        Buffer                mIndexBuffer[SwapChain::sMaxFramesInFlight];
        Buffer                mVertexBuffer[SwapChain::sMaxFramesInFlight];
        bool                  mOptimizeVertices = true;
        uint32_t              mCurrentBuffer    = 0;
        bool                  mHostVisible      = false;

        bool LoadFromVertices();
        void OptimizeVertices();
        void Create( Device& _device );
        void Destroy( Device& _device );
        bool RayCast( const Ray _ray, RaycastResult& _outResult ) const;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct Mesh : public Resource
    {
    FAN_RESOURCE( Mesh );

        bool RayCast( const Ray _ray, RaycastResult& _outResult ) const;
        bool LoadFromFile( const std::string& _path );

        void GenerateBoundingVolumes();
        bool Empty() const;
        int CountVertices() const;

        std::vector<SubMesh> mSubMeshes;

        ConvexHull mConvexHull;
        Sphere     mBoundingSphere;
        bool       mAutoGenerateBoundingVolumes = true;
    };
}