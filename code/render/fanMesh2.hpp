#pragma once

#include "core/shapes/fanConvexHull.hpp"
#include "core/resources/fanResource.hpp"
#include "core/resources/fanResource.hpp"
#include "render/fanVertex.hpp"
#include "render/fanMeshManager.hpp"
#include "render/core/fanBuffer.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{

    struct Device;

    class MeshManager;

    //================================================================================================================================
    // 3D mesh composed of triangles
    // can have a convex hull computed for it
    //================================================================================================================================
    class Mesh2 : public Resource
    {
    public:

        bool
        RayCast( const btVector3 _origin, const btVector3 _direction, btVector3& _outIntersection ) const;
        bool LoadFromFile( const std::string& _path );
        bool LoadFromVertices( const std::vector<Vertex>& _vertices );
        void OptimizeVertices();
        void GenerateConvexHull();

        int                     mIndex = -1;
        std::string             mPath;
        std::vector<Vertex>     mVertices;
        std::vector<uint32_t>   mIndices;
        bool                    mHostVisible      = false ;
        bool                    mOptimizeVertices = true  ;
        bool                    mAutoUpdateHull   = true  ;
        ConvexHull              mConvexHull;
    };

    //================================================================================================================================
    // Resource pointer for meshes
    //================================================================================================================================
    class Mesh2Ptr : public ResourcePtr<Mesh2>
    {
    public:
        Mesh2Ptr( Mesh2* _mesh = nullptr ) : ResourcePtr<Mesh2>( _mesh ) {}

        void Init( const std::string _path ) { m_path = _path; }
        const std::string&	GetPath() const { return m_path;  }

        ResourcePtr& operator=( Mesh2* _resource ) { SetResource( _resource ); return *this; }
    private:
        std::string m_path;
    };
}