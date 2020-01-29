#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SceneInstantiate::SceneInstantiate( Scene & _scene ) : m_scene( _scene ) {}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject * SceneInstantiate::InstanciatePrefab( const Prefab& _prefab, Gameobject * _parent )
	{
		if ( _prefab.IsEmpty() )
		{
			Debug::Warning() << "Failed to instantiate prefab" << Debug::Endl();
			return nullptr;
		}
		else
		{
			return InstantiateJson( _prefab.GetJsonGameobject(), _parent );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject *  SceneInstantiate::InstantiateJson( const Json& _json, Gameobject * _parent )
	{
		m_newGameobjectPtr.clear();
		m_newComponentPtr.clear();
		m_remapTable.clear();

		Gameobject::s_setIDfailed.Connect( &SceneInstantiate::OnSetIDFailed, this );
		GameobjectPtr::s_onCreateUnresolved.Connect ( &SceneInstantiate::OnGameobjectPtrCreate, this );
		ComponentIDPtr::s_onCreateUnresolved.Connect( &SceneInstantiate::OnComponentIDPtrCreate, this );

		// Load gameobject
		Gameobject * gameobject = m_scene.CreateGameobject( "tmp", _parent, false );
		gameobject->Load( _json );

		Gameobject::s_setIDfailed.Disconnect( &SceneInstantiate::OnSetIDFailed, this );
		GameobjectPtr::s_onCreateUnresolved.Disconnect( &SceneInstantiate::OnGameobjectPtrCreate, this );
		ComponentIDPtr::s_onCreateUnresolved.Disconnect( &SceneInstantiate::OnComponentIDPtrCreate, this );

		ResolvePointers();

		return gameobject;
	}

	//================================================================================================================================
	// Resolves component & gameobject pointers
	//================================================================================================================================
	void  SceneInstantiate::ResolvePointers()
	{
		// Resolves gameobjects
		for ( int goPtrIndex = 0; goPtrIndex < m_newGameobjectPtr.size(); goPtrIndex++ )
		{
			GameobjectPtr * ptr = m_newGameobjectPtr[goPtrIndex];

			auto it = m_remapTable.find( ptr->GetID() );
			const uint64_t index = ( it != m_remapTable.end() ? it->second : ptr->GetID() );
			Gameobject * gameobject = m_scene.FindGameobject( index );

			if ( gameobject != nullptr )
			{
				*ptr =  GameobjectPtr( gameobject, index );
			}
			else
			{
				Debug::Warning() << "Resolve gameobject failed for index " << index << Debug::Endl();
				*ptr =  GameobjectPtr();
			}
		}

		// Resolves components
		for ( int componentPtrIndex = 0; componentPtrIndex < m_newComponentPtr.size(); componentPtrIndex++ )
		{
			ComponentIDPtr * ptr = m_newComponentPtr[componentPtrIndex];

			auto it = m_remapTable.find( ptr->GetID().gameobjectID );
			const uint64_t index = ( it != m_remapTable.end() ? it->second : ptr->GetID().gameobjectID );
			Gameobject * gameobject =m_scene.FindGameobject( index );
			if ( gameobject != nullptr )
			{
				Component * component = gameobject->GetComponent( ptr->GetID().componentID );
				*ptr = ComponentIDPtr( component, IDPtrData( index, ptr->GetID().componentID ) );
			}
			else
			{
				Debug::Warning() << "Resolve component failed for gameobject index " << index << Debug::Endl();
				*ptr = ComponentIDPtr();
			}
		}
	}

	//================================================================================================================================
	// Saves the duplicates ids for future remap
	//================================================================================================================================
	void SceneInstantiate::OnSetIDFailed( uint64_t _id, Gameobject * _gameobject )
	{
		assert( _id != 0 );
		const uint64_t remapID = m_scene.GetUniqueID();
		_gameobject->SetUniqueID( remapID );
		m_remapTable[_id] = remapID;
		//Debug::Log() << "remapped id " << _id << " to id " << remapID << Debug::Endl();
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::OnGameobjectPtrCreate( GameobjectPtr * _ptr )
	{
		if ( _ptr->GetID() != 0 )
		{
			m_newGameobjectPtr.push_back( _ptr );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::OnComponentIDPtrCreate( ComponentIDPtr * _ptr )
	{
		if ( _ptr->GetID().gameobjectID != 0 )
		{
			m_newComponentPtr.push_back( _ptr );
		}
	}
}