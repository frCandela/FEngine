#include "engine/singletons/fanRenderDebug.hpp"
#include "core/math/fanMathUtils.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/fanDebug.hpp"
#include "core/math/fanVector3.hpp"
#include "engine/physics/fanFxTransform.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    void RenderDebug::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        RenderDebug& renderDebug = static_cast<RenderDebug&>( _component );
        (void)renderDebug;
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::Clear()
    {
        mDebugLines.clear();
        mDebugLinesNoDepthTest.clear();
        mDebugTriangles.clear();
        mDebugLines2D.clear();
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugPoint( const Vector3& _pos, const Fixed size, const Color _color, const bool _depthTestEnable )
    {
        DebugLine( _pos - size * Vector3::sUp, _pos + size * Vector3::sUp, _color, _depthTestEnable );
        DebugLine( _pos - size * Vector3::sLeft, _pos + size * Vector3::sLeft, _color, _depthTestEnable );
        DebugLine( _pos - size * Vector3::sForward, _pos + size * Vector3::sForward, _color, _depthTestEnable );
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugLine( const Vector3& _start, const Vector3& _end, const Color _color, const bool _depthTestEnable )
    {
        if( _depthTestEnable )
        {
            mDebugLines.push_back( DebugVertex( Math::ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
            mDebugLines.push_back( DebugVertex( Math::ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
        }
        else
        {
            mDebugLinesNoDepthTest.push_back( DebugVertex( Math::ToGLM( _start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
            mDebugLinesNoDepthTest.push_back( DebugVertex( Math::ToGLM( _end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
        }
    }

    //========================================================================================================
    // takes a list of triangle and a list of colors
    // 3 vertices per triangle
    // 1 color per triangle
    //========================================================================================================
    void
    RenderDebug::DebugTriangles( const std::vector<Vector3>& _triangles, const std::vector<Color>& _colors )
    {
        fanAssert( _triangles.size() % 3 == 0 );
        fanAssert( _colors.size() == _triangles.size() / 3 );

        mDebugTriangles.resize( mDebugTriangles.size() + _triangles.size() );
        for( int triangleIndex = 0; triangleIndex < (int)_triangles.size() / 3; triangleIndex++ )
        {
            const Vector3   v0     = _triangles[3 * triangleIndex + 0];
            const Vector3   v1     = _triangles[3 * triangleIndex + 1];
            const Vector3   v2     = _triangles[3 * triangleIndex + 2];
            const glm::vec3 normal = glm::normalize( Math::ToGLM( Vector3::Cross( v1 - v2, v0 - v2 ) ) );

            mDebugTriangles[3 * triangleIndex + 0] = DebugVertex( Math::ToGLM( v0 ), normal, _colors[triangleIndex].ToGLM() );
            mDebugTriangles[3 * triangleIndex + 1] = DebugVertex( Math::ToGLM( v1 ), normal, _colors[triangleIndex].ToGLM() );
            mDebugTriangles[3 * triangleIndex + 2] = DebugVertex( Math::ToGLM( v2 ), normal, _colors[triangleIndex].ToGLM() );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugCircle( const Vector3 _pos, const Fixed _radius, Vector3 _axis, uint32_t _nbSegments, const Color _color, const bool _depthTestEnable )
    {
        fanAssert( _nbSegments > 2 && _radius >= 0 );

        const Vector3 other      = Vector3( -_axis[1], -_axis[2], _axis[0] );
        Vector3       orthogonal = _radius * Vector3::Cross( _axis, other ).Normalized();
        const Fixed   angle      = 2 * FX_PI / _nbSegments;

        std::vector<DebugVertex>& lines = _depthTestEnable ? mDebugLines : mDebugLinesNoDepthTest;
        for( uint32_t segmentIndex = 0; segmentIndex < _nbSegments; segmentIndex++ )
        {

            Vector3 start = _pos + orthogonal;
            orthogonal = Quaternion::AngleAxis( angle, _axis ) * orthogonal;
            Vector3 end = _pos + orthogonal;

            lines.push_back( DebugVertex( Math::ToGLM( start ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
            lines.push_back( DebugVertex( Math::ToGLM( end ), glm::vec3( 0, 0, 0 ), _color.ToGLM() ) );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugTriangle( const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Color _color )
    {
        const glm::vec3 normal = glm::normalize( Math::ToGLM( Vector3::Cross( ( _v1 - _v2 ), ( _v0 - _v2 ) ) ) );
        mDebugTriangles.push_back( DebugVertex( Math::ToGLM( _v0 ), normal, _color.ToGLM() ) );
        mDebugTriangles.push_back( DebugVertex( Math::ToGLM( _v1 ), normal, _color.ToGLM() ) );
        mDebugTriangles.push_back( DebugVertex( Math::ToGLM( _v2 ), normal, _color.ToGLM() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugCube( const FxTransform& _transform, const Vector3& _halfExtent, const Color _color, const bool _depthTestEnable )
    {
        std::vector<Vector3> cube = GetCube( _halfExtent );

        for( int vertIndex = 0; vertIndex < (int)cube.size(); vertIndex++ )
        {
            cube[vertIndex] = _transform * cube[vertIndex];
        }

        glm::vec4 glmColor = _color.ToGLM();

        std::vector<DebugVertex>& lines = _depthTestEnable ? mDebugLines : mDebugLinesNoDepthTest;

        lines.push_back( DebugVertex( Math::ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );

        lines.push_back( DebugVertex( Math::ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );

        lines.push_back( DebugVertex( Math::ToGLM( cube[0] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[4] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[1] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[5] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[3] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[7] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[2] ), glm::vec3( 0, 0, 0 ), glmColor ) );
        lines.push_back( DebugVertex( Math::ToGLM( cube[6] ), glm::vec3( 0, 0, 0 ), glmColor ) );
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugIcoSphere( const FxTransform& _transform, const Fixed _radius, const int _numSubdivisions, const Color _color, const bool _depthTestEnable )
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
            DebugLine( v0, v1, _color, _depthTestEnable );
            DebugLine( v1, v2, _color, _depthTestEnable );
            DebugLine( v2, v0, _color, _depthTestEnable );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugSphere( const Vector3 _origin, const Fixed _radius, const Color _color, const bool _depthTestEnable )
    {
        DebugCircle( _origin, _radius, Vector3::sUp, 32, _color, _depthTestEnable );
        DebugCircle( _origin, _radius, Vector3::sLeft, 32, _color, _depthTestEnable );
        DebugCircle( _origin, _radius, Vector3::sForward, 32, _color, _depthTestEnable );
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugCone( const FxTransform& _transform, const Fixed _radius, const Fixed _height, const int _numSubdivisions, const Color _color )
    {
        std::vector<Vector3> cone = GetCone( _radius, _height, _numSubdivisions );

        for( int vertIndex = 0; vertIndex < (int)cone.size(); vertIndex++ )
        {
            cone[vertIndex] = _transform * cone[vertIndex];
        }

        for( int triangleIndex = 0; triangleIndex < (int)cone.size() / 3; triangleIndex++ )
        {
            DebugTriangle( cone[3 * triangleIndex + 0],
                           cone[3 * triangleIndex + 1],
                           cone[3 * triangleIndex + 2],
                           _color );
        }
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugAABB( const AABB& _aabb, const Color _color )
    {
        std::vector<Vector3> corners = _aabb.GetCorners();
        // Top
        DebugLine( corners[0], corners[1], _color );
        DebugLine( corners[1], corners[2], _color );
        DebugLine( corners[2], corners[3], _color );
        DebugLine( corners[3], corners[0], _color );
        // Bot
        DebugLine( corners[4], corners[5], _color );
        DebugLine( corners[5], corners[6], _color );
        DebugLine( corners[6], corners[7], _color );
        DebugLine( corners[7], corners[4], _color );
        //Vertical sides
        DebugLine( corners[0], corners[4], _color );
        DebugLine( corners[1], corners[5], _color );
        DebugLine( corners[2], corners[6], _color );
        DebugLine( corners[3], corners[7], _color );
    }

    //========================================================================================================
    // start/end in screen pixels
    //========================================================================================================
    void RenderDebug::DebugLine2D( const glm::ivec2 _start, const glm::ivec2 _end, const Color _color )
    {
        mDebugLines2D.push_back( DebugVertex2D( _start, _color.ToGLM() ) );
        mDebugLines2D.push_back( DebugVertex2D( _end, _color.ToGLM() ) );
    }

    //========================================================================================================
    //========================================================================================================
    void RenderDebug::DebugQuad2D( const glm::ivec2 _pos, const glm::ivec2 _size, const Color _color )
    {
        const glm::ivec2 tl = _pos;
        const glm::ivec2 tr = _pos + glm::ivec2( _size.x, 0 );
        const glm::ivec2 bl = _pos + glm::ivec2( 0, _size.y );
        const glm::ivec2 br = _pos + _size;

        DebugLine2D( tl, tr, _color );
        DebugLine2D( tr, br, _color );
        DebugLine2D( br, bl, _color );
        DebugLine2D( bl, tl, _color );
    }
}