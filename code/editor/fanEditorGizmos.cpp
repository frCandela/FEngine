#include "editor/fanEditorGizmos.hpp"
#include "editor/fanEditorDebug.hpp"
#include "core/math/shapes/fanRay.hpp"
#include "core/math/fanBasicModels.hpp"
#include "core/input/fanMouse.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/fanScene.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	EditorGizmos* EditorGizmos::s_editorGizmos = nullptr;

	//================================================================================================================================
	//================================================================================================================================
	EditorGizmos::EditorGizmos( Scene*& _currentScene )
		: m_currentScene( _currentScene )
	{}

	//================================================================================================================================
	// Returns the new position of the move gizmo
	// Caller must provide a unique ID to allow proper caching of the user input data
	//================================================================================================================================
	bool EditorGizmos::DrawMoveGizmo( const btTransform _transform, const size_t _uniqueID, btVector3& _newPosition )
	{
		GizmoCacheData& cacheData = m_gizmoCacheData[ _uniqueID ];
		const btVector3 origin = _transform.getOrigin();
		const btTransform rotation( _transform.getRotation() );
		const btVector3 axisDirection[ 3 ] = { btVector3( 1, 0, 0 ), btVector3( 0, 1, 0 ),  btVector3( 0, 0, 1 ) };
		const btVector3 cameraPosition = m_currentScene->GetMainCamera().GetGameobject().GetTransform().GetPosition();
		const float size = 0.2f * origin.distance( cameraPosition );
		const btTransform coneRotation[ 3 ] = {
		btTransform( btQuaternion( 0, 0, btRadians( -90 ) ), size * axisDirection[ 0 ] )
		,btTransform( btQuaternion::getIdentity(),		size * axisDirection[ 1 ] )
		,btTransform( btQuaternion( 0, btRadians( 90 ), 0 ), size * axisDirection[ 2 ] )
		};

		_newPosition = _transform.getOrigin();
		for ( int axisIndex = 0; axisIndex < 3; axisIndex++ )
		{
			const Color opaqueColor( axisDirection[ axisIndex ].x(), axisDirection[ axisIndex ].y(), axisDirection[ axisIndex ].z(), 1.f );

			// Generates a cone shape
			std::vector<btVector3> coneTris = GetCone( 0.1f * size, 0.5f * size, 10 );
			btTransform transform = _transform * coneRotation[ axisIndex ];
			for ( int vertIndex = 0; vertIndex < coneTris.size(); vertIndex++ )
			{
				coneTris[ vertIndex ] = transform * coneTris[ vertIndex ];
			}

			if ( ImGui::IsMouseReleased( 0 ) )
			{
				cacheData.pressed = false;
				cacheData.axisIndex = -1;
			}

			// Raycast on the gizmo shape to determine if the mouse is hovering it
			Color clickedColor = opaqueColor;
			const Ray ray = m_currentScene->GetMainCamera().ScreenPosToRay( Mouse::Get().GetScreenSpacePosition() );
			for ( int triIndex = 0; triIndex < coneTris.size() / 3; triIndex++ )
			{
				Triangle triangle( coneTris[ 3 * triIndex + 0 ], coneTris[ 3 * triIndex + 1 ], coneTris[ 3 * triIndex + 2 ] );
				btVector3 intersection;
				if ( triangle.RayCast( ray.origin, ray.direction, intersection ) )
				{
					clickedColor[ 3 ] = 0.5f;
					if ( Mouse::Get().GetButtonPressed( 0 ) )
					{
						cacheData.pressed = true;
						cacheData.axisIndex = axisIndex;
					}
					break;
				}
			}

			// Draw the gizmo cone & lines
			RendererDebug::Get().DebugLine( origin, origin + size * ( _transform * axisDirection[ axisIndex ] - origin ), opaqueColor, false );
			for ( int triangleIndex = 0; triangleIndex < coneTris.size() / 3; triangleIndex++ )
			{
				RendererDebug::Get().DebugTriangle( coneTris[ 3 * triangleIndex + 0 ], coneTris[ 3 * triangleIndex + 1 ], coneTris[ 3 * triangleIndex + 2 ], clickedColor );
			}

			// Calculate closest point between the mouse ray and the axis selected
			if ( cacheData.pressed == true && cacheData.axisIndex == axisIndex )
			{
				btVector3 axis = rotation * axisDirection[ axisIndex ];

				const Ray screenRay = m_currentScene->GetMainCamera().ScreenPosToRay( Mouse::Get().GetScreenSpacePosition() );
				const Ray axisRay = { origin , axis };
				btVector3 trash, projectionOnAxis;
				screenRay.RayClosestPoints( axisRay, trash, projectionOnAxis );

				if ( Mouse::Get().GetButtonPressed( 0 ) )
				{
					cacheData.offset = projectionOnAxis - _transform.getOrigin();
				}

				_newPosition = projectionOnAxis - cacheData.offset;
			}
		}
		return cacheData.pressed;
	}
}