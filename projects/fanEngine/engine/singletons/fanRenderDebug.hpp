#pragma once

#include "LinearMath/btTransform.h"
#include "core/shapes/fanAABB.hpp"
#include "core/ecs/fanEcsSingleton.hpp"
#include "render/fanRenderer.hpp"
#include "render/resources/fanMesh.hpp"
#include "render/fanVertex.hpp"

namespace fan
{
    struct Vector3;

    //========================================================================================================
    // contains all the render data of the world for the renderer
    //========================================================================================================
    struct RenderDebug : public EcsSingleton
    {
        ECS_SINGLETON( RenderDebug )
        static void SetInfo( EcsSingletonInfo& _info );
        static void Init( EcsWorld& _world, EcsSingleton& _component );

        // vertex data
        std::vector<DebugVertex>   mDebugLines;
        std::vector<DebugVertex>   mDebugLinesNoDepthTest;
        std::vector<DebugVertex>   mDebugTriangles;
        std::vector<DebugVertex2D> mDebugLines2D;

        void Clear();
        void DebugPoint( const btVector3 _pos, const float size, const Color _color, const bool _depthTestEnable = false );
        void DebugPoint( const Vector3& _pos, const float size, const Color _color, const bool _depthTestEnable = false );
        void DebugLine( const btVector3 _start, const btVector3 _end, const Color _color, const bool _depthTestEnable = false );
        void DebugLine( const Vector3& _start, const Vector3& _end, const Color _color, const bool _depthTestEnable = false );
        void DebugTriangle( const btVector3 _v0, const btVector3 _v1, const btVector3 _v2, const Color _color );
        void DebugTriangle( const Vector3 _v0, const Vector3 _v1, const Vector3 _v2, const Color _color );
        void DebugTriangles( const std::vector<btVector3>& _triangles, const std::vector<Color>& _colors );
        void DebugCircle( const btVector3 _pos, const float _radius, btVector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable = false );
        void DebugCube( const btTransform _transform, const btVector3 _halfExtent, const Color _color, const bool _depthTestEnable = false );
        void DebugIcoSphere( const btTransform _transform, const float _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable = false );
        void DebugSphere( const btVector3 _origin, const float _radius, const Color _color, const bool _depthTestEnable = false );
        void DebugCone( const btTransform _transform, const float _radius, const float _height, const int _numSubdivisions, const Color _color );
        void DebugAABB( const AABB& _aabb, const Color _color );
        void DebugLine2D( const glm::ivec2 _start, const glm::ivec2 _end, const Color _color );
        void DebugQuad2D( const glm::ivec2 _pos, const glm::ivec2 _size, const Color _color );
    };
}