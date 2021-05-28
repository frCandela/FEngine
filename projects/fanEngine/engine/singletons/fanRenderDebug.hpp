#pragma once

#include "core/shapes/fanAABB.hpp"
#include "core/ecs/fanEcsSingleton.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanVertex.hpp"
#include "core/math/fanVector3.hpp"

namespace fan
{
    struct Vector3;
    struct Transform;

    //==================================================================================================================================================================================================
    // contains all the render data of the world for the renderer
    //==================================================================================================================================================================================================
    struct RenderDebug : public EcsSingleton
    {
    ECS_SINGLETON( RenderDebug )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        // vertex data
        std::vector<DebugLineVertex> mDebugLines;
        std::vector<DebugLineVertex> mDebugLinesNoDepthTest;
        std::vector<DebugVertex>     mDebugTriangles;
        std::vector<DebugVertex2D>   mDebugLines2D;

        void Clear();
        void DebugPoint( const Vector3& _pos, const Fixed size, const Color _color, const bool _depthTestEnable = false );
        void DebugLine( const Vector3& _start, const Vector3& _end, const Color _color, const bool _depthTestEnable = false );
        void DebugTriangle( const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Color _color );
        void DebugTriangles( const std::vector<Vector3>& _triangles, const std::vector<Color>& _colors );
        void DebugCircle( const Vector3 _pos, const Fixed _radius, Vector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable = false );
        void DebugCube( const Transform& _transform, const Vector3& _halfExtent, const Color _color, const bool _depthTestEnable = false );
        void DebugIcoSphere( const Transform& _transform, const Fixed _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable = false );
        void DebugSphere( const Vector3 _origin, const Fixed _radius, const Color _color, const bool _depthTestEnable = false );
        void DebugCone( const Transform& _transform, const Fixed _radius, const Fixed _height, const int _numSubdivisions, const Color _color );
        void DebugAABB( const AABB& _aabb, const Color _color );
        void DebugLine2D( const glm::ivec2 _start, const glm::ivec2 _end, const Color _color );
        void DebugQuad2D( const glm::ivec2 _pos, const glm::ivec2 _size, const Color _color );
    };
}