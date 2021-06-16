#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResourcePtr.hpp"
#include "core/shapes/fanSphere.hpp"
#include "core/math/fanMatrix4.hpp"
#include "core/fanString.hpp"
#include "render/fanVertex.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/core/fanSwapChain.hpp"
#include "render/resources/fanSkeleton.hpp"

struct Device;

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SubSkinnedMesh
    {
        std::vector<VertexSkinned> mVertices;
        std::vector<uint32_t>      mIndices;
        Buffer                     mIndexBuffer;
        Buffer                     mVertexBuffer;
        bool                       mOptimizeVertices = true;
        bool                       mHostVisible      = false;

        bool LoadFromVertices();
        void OptimizeVertices();
        void Create( Device& _device );
        void Destroy( Device& _device );
        bool RayCast( const Ray _ray, RaycastResult& _outResult ) const;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct SkinnedMesh : public Resource
    {
    FAN_RESOURCE( SkinnedMesh );

        bool RayCast( const Ray _ray, RaycastResult& _outResult ) const;
        bool LoadFromFile( const std::string& _path );

        void GenerateBoundingVolumes();
        bool Empty() const;
        int CountVertices() const;

        std::vector<SubSkinnedMesh> mSubMeshes;
        Skeleton                    mSkeleton;

        ConvexHull mConvexHull;
        Sphere     mBoundingSphere;
        bool       mAutoGenerateBoundingVolumes = true;
    };
}