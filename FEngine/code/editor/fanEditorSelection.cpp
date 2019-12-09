#include "fanGlobalIncludes.h"
#include "editor/fanEditorSelection.h"

#include "scene/fanGameobject.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "core/time/fanProfiler.h"
#include "core/input/fanMouse.h"
#include "core/math/shapes/fanRay.h"
#include "scene/fanScene.h"
#include "scene/components/ui/fanUIMeshRenderer.h"
#include "scene/components/ui/fanUITransform.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMeshRenderer.h"
#include "scene/components/fanCamera.h"
#include "scene/components/fanTransform.h"
#include "renderer/fanMesh.h"
#include "editor/fanEditorGizmos.h"
#include "editor/fanEditorDebug.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================	
	EditorSelection::EditorSelection( Scene*& _currentScene ) : m_currentScene( _currentScene ) {}

	//================================================================================================================================
	//================================================================================================================================	
	void EditorSelection::ConnectCallbacks(  Scene& _clientScene, Scene& _serverScene )
	{
		Input::Get().Manager().FindEvent( "delete" )->Connect(	&EditorSelection::DeleteSelection, this );
		_clientScene.onDeleteGameobject.Connect( &EditorSelection::OnGameobjectDeleted, this );
		_serverScene.onDeleteGameobject.Connect( &EditorSelection::OnGameobjectDeleted, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::DeleteSelection()
	{
		if ( m_selectedGameobject != nullptr )
		{
			m_currentScene->DeleteGameobject( m_selectedGameobject );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Update( const bool _gameWindowHovered )
	{
		SCOPED_PROFILE( selection )

			bool mouseCaptured = ImGui::GetIO().WantCaptureMouse;

		// Translation gizmo on selected gameobject
		if ( m_selectedGameobject != nullptr && m_selectedGameobject != m_currentScene->GetMainCamera()->GetGameobject()
			&& m_selectedGameobject->GetComponent<UIMeshRenderer>() == nullptr
			&& m_selectedGameobject->GetComponent<UITransform>() == nullptr )

		{
			Transform * transform = m_selectedGameobject->GetComponent< Transform >();
			btVector3 newPosition;
			if ( EditorDebug::Get().Gizmos().DrawMoveGizmo( btTransform( btQuaternion( 0, 0, 0 ), transform->GetPosition() ), (size_t)this, newPosition ) )
			{
				transform->SetPosition( newPosition );
				mouseCaptured = true;
			}
		}

		// Mouse selection
		if ( _gameWindowHovered && Mouse::Get().GetButtonPressed( Mouse::button0 ) )
		{
			const btVector3 cameraOrigin = m_currentScene->GetMainCamera()->GetGameobject()->GetComponent<Transform>()->GetPosition();
			const Ray ray = m_currentScene->GetMainCamera()->ScreenPosToRay( Mouse::Get().GetScreenSpacePosition() );
			const std::vector<Gameobject *>  & entities = m_currentScene->BuildEntitiesList();

			// raycast on all the entities
			Gameobject * closestGameobject = nullptr;
			float closestDistance2 = std::numeric_limits<float>::max();
			for ( int gameobjectIndex = 0; gameobjectIndex < entities.size(); gameobjectIndex++ )
			{
				Gameobject * gameobject = entities[gameobjectIndex];

				if ( gameobject == m_currentScene->GetMainCamera()->GetGameobject() )
				{
					continue;
				}

				const AABB & aabb = gameobject->GetAABB();
				btVector3 intersection;
				if ( aabb.RayCast( ray.origin, ray.direction, intersection ) == true )
				{
					MeshRenderer * meshRenderer = gameobject->GetComponent<MeshRenderer>();
					if ( meshRenderer != nullptr && meshRenderer->GetMesh() != nullptr )
					{
						Transform * transform = gameobject->GetComponent<Transform>();
						const Ray transformedRay( transform->InverseTransformPoint( ray.origin ), transform->InverseTransformDirection( ray.direction ) );
						if ( meshRenderer->GetMesh()->GetHull().RayCast( transformedRay.origin, transformedRay.direction, intersection ) == false )
						{
							continue;
						}
					}
					const float distance2 = intersection.distance2( cameraOrigin );
					if ( distance2 < closestDistance2 )
					{
						closestDistance2 = distance2;
						closestGameobject = gameobject;
					}
				}
			}
			SetSelectedGameobject( closestGameobject );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::SetSelectedGameobject( Gameobject * _selectedGameobject )
	{
		m_selectedGameobject = _selectedGameobject;
		onGameobjectSelected.Emmit( m_selectedGameobject );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::Deselect()
	{
		m_selectedGameobject = nullptr;
		onGameobjectSelected.Emmit( m_selectedGameobject );
	}

	//================================================================================================================================
	//================================================================================================================================
	void EditorSelection::OnGameobjectDeleted( Gameobject * _gameobject )
	{
		if ( _gameobject == m_selectedGameobject )
		{
			Deselect();
		}
	}
}