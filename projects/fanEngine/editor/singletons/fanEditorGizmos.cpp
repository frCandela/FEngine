#include "editor/singletons/fanEditorGizmos.hpp"

#include "core/shapes/fanRay.hpp"
#include "core/shapes/fanTriangle.hpp"
#include "core/math/fanBasicModels.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "engine/components/fanSceneNode.hpp"
#include "engine/physics/fanTransform.hpp"
#include "engine/components/fanCamera.hpp"
#include "engine/singletons/fanRenderDebug.hpp"
#include "engine/singletons/fanMouse.hpp"
#include "engine/singletons/fanScene.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGizmos::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void EditorGizmos::Init( EcsWorld& _world, EcsSingleton& _component )
    {
        EditorGizmos& editorGizmos = static_cast<EditorGizmos&>( _component );
        editorGizmos.mWorld = &_world;
    }

    //==================================================================================================================================================================================================
    // Returns the new position of the move gizmo
    // Caller must provide a unique ID to allow proper caching of the user input data
    //==================================================================================================================================================================================================
    bool EditorGizmos::DrawMoveGizmo( const Transform& _transform, const size_t _uniqueID, Vector3& _newPosition )
    {
        Scene& scene = mWorld->GetSingleton<Scene>();
        Mouse& mouse = mWorld->GetSingleton<Mouse>();

        // Get main camera data
        EcsWorld& world = *scene.mWorld;
        const EcsEntity id = world.GetEntity( scene.mMainCameraHandle );
        const Transform& cameraTransform = world.GetComponent<Transform>( id );
        const Camera   & camera          = world.GetComponent<Camera>( id );

        GizmoCacheData& cacheData = mGizmoCacheData[_uniqueID];
        const Vector3   origin           = _transform.mPosition;
        const Vector3   axisDirection[3] = { Vector3( 1, 0, 0 ),
                                             Vector3( 0, 1, 0 ),
                                             Vector3( 0, 0, 1 ) };
        const Vector3   cameraPosition   = cameraTransform.mPosition;
        const Fixed     size             = FIXED( 0.2 ) * Vector3::Distance( origin, cameraPosition );
        const Transform coneRotation[3]  = {
                Transform::Make( Quaternion::Euler( 0, 0, -90 ), size * axisDirection[0] ),
                Transform::Make( Quaternion::sIdentity, size * axisDirection[1] ),
                Transform::Make( Quaternion::Euler( 90, 0, 0 ), size * axisDirection[2] )
        };

        _newPosition = _transform.mPosition;
        for( int axisIndex = 0; axisIndex < 3; axisIndex++ )
        {
            const Color opaqueColor( axisDirection[axisIndex].x.ToFloat(),
                                     axisDirection[axisIndex].y.ToFloat(),
                                     axisDirection[axisIndex].z.ToFloat(),
                                     1.f );

            // Generates a cone shape
            std::vector<Vector3> coneTris  = GetCone( FIXED( 0.1 ) * size, FIXED( 0.5 ) * size, 10 );
            Transform            transform = _transform * coneRotation[axisIndex];
            for( int             vertIndex = 0; vertIndex < (int)coneTris.size(); vertIndex++ )
            {
                coneTris[vertIndex] = transform * coneTris[vertIndex];
            }

            if( ImGui::IsMouseReleased( 0 ) )
            {
                cacheData.pressed   = false;
                cacheData.axisIndex = -1;
            }

            // Raycast on the gizmo shape to determine if the mouse is hovering it
            Color     clickedColor = opaqueColor;
            const Ray ray          = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
            for( int  triIndex     = 0; triIndex < (int)coneTris.size() / 3; triIndex++ )
            {
                Triangle triangle( coneTris[3 * triIndex + 0], coneTris[3 * triIndex + 1], coneTris[3 * triIndex + 2] );
                RaycastResult  result;
                if( triangle.RayCast( ray, result ) )
                {
                    clickedColor[3] = 0.5f;
                    if( mouse.mPressed[Mouse::buttonLeft] )
                    {
                        cacheData.pressed   = true;
                        cacheData.axisIndex = axisIndex;
                    }
                    break;
                }
            }

            // Draw the gizmo cone & lines
            RenderDebug& rd = world.GetSingleton<RenderDebug>();
            rd.DrawLine( origin, origin + size * ( _transform.TransformPoint( axisDirection[axisIndex] ) - origin ), opaqueColor, false );
            for( int triangleIndex = 0; triangleIndex < (int)coneTris.size() / 3; triangleIndex++ )
            {
                rd.DrawTriangle( coneTris[3 * triangleIndex + 0], coneTris[3 * triangleIndex + 1], coneTris[3 * triangleIndex + 2], clickedColor, false );
            }

            // Calculate closest point between the mouse ray and the axis selected
            if( cacheData.pressed == true && cacheData.axisIndex == axisIndex )
            {
                Vector3 axis = _transform.TransformDirection( axisDirection[axisIndex] );

                const Ray screenRay = camera.ScreenPosToRay( cameraTransform, mouse.LocalScreenSpacePosition() );
                const Ray axisRay   = { origin, axis };
                Vector3   trash, projectionOnAxis;
                screenRay.RayClosestPoints( axisRay, trash, projectionOnAxis );

                if( mouse.mPressed[Mouse::buttonLeft] )
                {
                    cacheData.offset = projectionOnAxis - _transform.mPosition;
                }

                _newPosition = projectionOnAxis - cacheData.offset;
            }
        }
        return cacheData.pressed;
    }
}