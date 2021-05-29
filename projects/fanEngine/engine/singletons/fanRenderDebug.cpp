#include "engine/singletons/fanRenderDebug.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanVector3.hpp"
#include "engine/physics/fanTransform.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        RenderDebug& renderDebug = static_cast<RenderDebug&>( _component );
        renderDebug.Clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::Clear()
    {
        mDebugLines.clear();
        mDebugLinesNoDepthTest.clear();
        mDebugTriangles.clear();
        mDebugTrianglesNoDepthTest.clear();
        mDebugLines2D.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawPoint( const Vector3& _pos, const Fixed size, const Color _color, const bool _depthTestEnable )
    {
        DrawLine( _pos - size * Vector3::sUp, _pos + size * Vector3::sUp, _color, _depthTestEnable );
        DrawLine( _pos - size * Vector3::sLeft, _pos + size * Vector3::sLeft, _color, _depthTestEnable );
        DrawLine( _pos - size * Vector3::sForward, _pos + size * Vector3::sForward, _color, _depthTestEnable );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawLine( const Vector3& _start, const Vector3& _end, const Color _color, const bool _depthTestEnable )
    {
        if( _depthTestEnable )
        {
            mDebugLines.push_back( DebugLineVertex( _start.ToGlm(), _color.ToGLM() ) );
            mDebugLines.push_back( DebugLineVertex( _end.ToGlm(), _color.ToGLM() ) );
        }
        else
        {
            mDebugLinesNoDepthTest.push_back( DebugLineVertex( _start.ToGlm(), _color.ToGLM() ) );
            mDebugLinesNoDepthTest.push_back( DebugLineVertex( _end.ToGlm(), _color.ToGLM() ) );
        }
    }

    //==================================================================================================================================================================================================
    // takes a list of triangle and a list of colors
    // 3 vertices per triangle
    // 1 color per triangle
    //==================================================================================================================================================================================================
    void RenderDebug::DrawTriangles( const std::vector<Vector3>& _triangles, const std::vector<Color>& _colors, const bool _depthTestEnable )
    {
        fanAssert( _triangles.size() % 3 == 0 );
        fanAssert( _colors.size() == _triangles.size() / 3 );

        std::vector<DebugVertex>& triangles = _depthTestEnable ? mDebugTriangles : mDebugTrianglesNoDepthTest;
        triangles.resize( mDebugTriangles.size() + _triangles.size() );
        for( int triangleIndex = 0; triangleIndex < (int)_triangles.size() / 3; triangleIndex++ )
        {
            const Vector3   v0     = _triangles[3 * triangleIndex + 0];
            const Vector3   v1     = _triangles[3 * triangleIndex + 1];
            const Vector3   v2     = _triangles[3 * triangleIndex + 2];
            const glm::vec3 normal = glm::normalize( Vector3::Cross( v1 - v2, v0 - v2 ).ToGlm() );

            triangles[3 * triangleIndex + 0] = DebugVertex( v0.ToGlm(), normal, _colors[triangleIndex].ToGLM() );
            triangles[3 * triangleIndex + 1] = DebugVertex( v1.ToGlm(), normal, _colors[triangleIndex].ToGLM() );
            triangles[3 * triangleIndex + 2] = DebugVertex( v2.ToGlm(), normal, _colors[triangleIndex].ToGLM() );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawCircle( const Vector3 _pos, const Fixed _radius, Vector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable )
    {
        fanAssert( _nbSegments > 2 && _radius >= 0 );

        const Vector3 other          = Vector3( -_axis[1], -_axis[2], _axis[0] );
        const Vector3 orthogonal     = _radius * Vector3::Cross( _axis, other ).Normalized();
        const Fixed   angleIncrement = Fixed::sTwoPi / _nbSegments;
        Fixed         angle          = 0;

        std::vector<DebugLineVertex>& lines = _depthTestEnable ? mDebugLines : mDebugLinesNoDepthTest;
        for( uint32_t segmentIndex = 0; segmentIndex < _nbSegments; segmentIndex++ )
        {

            Vector3 start = _pos + Quaternion::AngleAxisRadians( angle, _axis ) * orthogonal;
            angle += angleIncrement;
            Vector3 end = _pos + Quaternion::AngleAxisRadians( angle, _axis ) * orthogonal;

            lines.push_back( DebugLineVertex( start.ToGlm(), _color.ToGLM() ) );
            lines.push_back( DebugLineVertex( end.ToGlm(), _color.ToGLM() ) );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawTriangle( const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Color _color, const bool _depthTestEnable )
    {
        std::vector<DebugVertex>& triangles = _depthTestEnable ? mDebugTriangles : mDebugTrianglesNoDepthTest;
        const glm::vec3 normal = glm::normalize( Vector3::Cross( _v1 - _v2, _v0 - _v2 ).ToGlm() );
        triangles.push_back( DebugVertex( _v0.ToGlm(), normal, _color.ToGLM() ) );
        triangles.push_back( DebugVertex( _v1.ToGlm(), normal, _color.ToGLM() ) );
        triangles.push_back( DebugVertex( _v2.ToGlm(), normal, _color.ToGLM() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawCube( const Transform& _transform, const Vector3& _halfExtent, const Color _color, const bool _depthTestEnable )
    {
        std::vector<Vector3> cube = GetCube( _halfExtent );

        for( int vertIndex = 0; vertIndex < (int)cube.size(); vertIndex++ )
        {
            cube[vertIndex] = _transform * cube[vertIndex];
        }

        glm::vec4 glmColor = _color.ToGLM();

        std::vector<DebugLineVertex>& lines = _depthTestEnable ? mDebugLines : mDebugLinesNoDepthTest;

        lines.push_back( DebugLineVertex( cube[0].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[1].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[1].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[3].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[3].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[2].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[2].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[0].ToGlm(), glmColor ) );

        lines.push_back( DebugLineVertex( cube[4].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[5].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[5].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[7].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[7].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[6].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[6].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[4].ToGlm(), glmColor ) );

        lines.push_back( DebugLineVertex( cube[0].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[4].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[1].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[5].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[3].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[7].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[2].ToGlm(), glmColor ) );
        lines.push_back( DebugLineVertex( cube[6].ToGlm(), glmColor ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawIcoSphere( const Transform& _transform, const Fixed _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable )
    {
        if( _radius <= 0 )
        {
            Debug::Warning( "Debug sphere radius cannot be zero or negative" );
            return;
        }

        std::vector<Vector3> sphere = GetSphere( _radius, _numSubdivisions );

        for( int vertIndex = 0; vertIndex < (int)sphere.size(); vertIndex++ )
        {
            sphere[vertIndex] = _transform * sphere[vertIndex];
        }

        for( int triangleIndex = 0; triangleIndex < (int)sphere.size() / 3; triangleIndex++ )
        {
            const Vector3 v0 = sphere[3 * triangleIndex + 0];
            const Vector3 v1 = sphere[3 * triangleIndex + 1];
            const Vector3 v2 = sphere[3 * triangleIndex + 2];
            DrawLine( v0, v1, _color, _depthTestEnable );
            DrawLine( v1, v2, _color, _depthTestEnable );
            DrawLine( v2, v0, _color, _depthTestEnable );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawSphere( const Vector3 _origin, const Fixed _radius, const Color _color, const bool _depthTestEnable )
    {
        DrawCircle( _origin, _radius, Vector3::sUp, 32, _color, _depthTestEnable );
        DrawCircle( _origin, _radius, Vector3::sLeft, 32, _color, _depthTestEnable );
        DrawCircle( _origin, _radius, Vector3::sForward, 32, _color, _depthTestEnable );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawCone( const Transform& _transform, const Fixed _radius, const Fixed _height, const int _numSubdivisions, const Color _color, const bool _depthTestEnable )
    {
        std::vector<Vector3> cone = GetCone( _radius, _height, _numSubdivisions );

        for( int vertIndex = 0; vertIndex < (int)cone.size(); vertIndex++ )
        {
            cone[vertIndex] = _transform * cone[vertIndex];
        }

        for( int triangleIndex = 0; triangleIndex < (int)cone.size() / 3; triangleIndex++ )
        {
            DrawTriangle( cone[3 * triangleIndex + 0], cone[3 * triangleIndex + 1], cone[3 * triangleIndex + 2], _color, _depthTestEnable );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawAABB( const AABB& _aabb, const Color _color )
    {
        std::vector<Vector3> corners = _aabb.GetCorners();
        // Top
        DrawLine( corners[0], corners[1], _color );
        DrawLine( corners[1], corners[2], _color );
        DrawLine( corners[2], corners[3], _color );
        DrawLine( corners[3], corners[0], _color );
        // Bot
        DrawLine( corners[4], corners[5], _color );
        DrawLine( corners[5], corners[6], _color );
        DrawLine( corners[6], corners[7], _color );
        DrawLine( corners[7], corners[4], _color );
        //Vertical sides
        DrawLine( corners[0], corners[4], _color );
        DrawLine( corners[1], corners[5], _color );
        DrawLine( corners[2], corners[6], _color );
        DrawLine( corners[3], corners[7], _color );
    }

    //==================================================================================================================================================================================================
    // start/end in screen pixels
    //==================================================================================================================================================================================================
    void RenderDebug::DrawLine2D( const glm::ivec2 _start, const glm::ivec2 _end, const Color _color )
    {
        mDebugLines2D.push_back( DebugVertex2D( _start, _color.ToGLM() ) );
        mDebugLines2D.push_back( DebugVertex2D( _end, _color.ToGLM() ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void RenderDebug::DrawQuad2D( const glm::ivec2 _pos, const glm::ivec2 _size, const Color _color )
    {
        const glm::ivec2 tl = _pos;
        const glm::ivec2 tr = _pos + glm::ivec2( _size.x, 0 );
        const glm::ivec2 bl = _pos + glm::ivec2( 0, _size.y );
        const glm::ivec2 br = _pos + _size;

        DrawLine2D( tl, tr, _color );
        DrawLine2D( tr, br, _color );
        DrawLine2D( br, bl, _color );
        DrawLine2D( bl, tl, _color );
    }
}