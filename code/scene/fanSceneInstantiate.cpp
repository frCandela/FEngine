#include "scene/fanSceneInstantiate.hpp"
#include "scene/fanGameobject.hpp"
#include "scene/components/fanComponent.hpp"
#include "scene/fanPrefab.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SceneInstantiate::SceneInstantiate( Scene& _scene ) : m_scene( _scene ) {}

	//================================================================================================================================
	//================================================================================================================================
	Gameobject* SceneInstantiate::InstanciatePrefab( const Prefab& _prefab, Gameobject& _parent )
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
	Gameobject* SceneInstantiate::InstantiateJson( const Json& _json, Gameobject& _parent )
	{
		const uint64_t idOffset = m_scene.GetNextUniqueID() - 1;

		// Load gameobject
		uint64_t id;
		Serializable::LoadUInt64( _json, "gameobject_id", id );
		Gameobject* gameobject = m_scene.CreateGameobject( "tmp", &_parent, id + idOffset );
		gameobject->Load( _json, idOffset );

		ResolveGameobjectPtr( idOffset );
		ResolveComponentPtr( idOffset );

		m_unresolvedGameobjectPtr.clear();
		m_newComponentPtr.clear();

		return gameobject;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::Clear()
	{
		while ( ! m_registeredGameobjectPtr.empty() )
		{
			* ( * m_registeredGameobjectPtr.begin() ) = nullptr;
		}
		assert( m_registeredGameobjectPtr.empty() );
	}

	//================================================================================================================================
	// Resolves gameobject pointers ( find by id in the scene )
	// _idOffset adds an offset to the gameobject ids being resolved, it allow for easier instantiation of prefabs
	//================================================================================================================================
	void  SceneInstantiate::ResolveGameobjectPtr( const uint64_t _idOffset )
	{
		// Resolves gameobjects
		for ( int goPtrIndex = 0; goPtrIndex < m_unresolvedGameobjectPtr.size(); goPtrIndex++ )
		{			
			GameobjectPtr& ptr = *m_unresolvedGameobjectPtr[goPtrIndex];	
			const uint64_t id = ptr.GetId() + _idOffset;
			Gameobject * gameobject = m_scene.FindGameobject( id );	
			assert( ! ptr.IsValid() && ptr.GetId() != 0 );

			if ( gameobject != nullptr )
			{
				ptr = gameobject;
			}
			else
			{
				Debug::Warning() << "Resolve gameobject failed for index " << id << Debug::Endl();
			}
		}
		m_unresolvedGameobjectPtr.clear();
	}

	//================================================================================================================================
	// Resolves gameobject pointers
	//================================================================================================================================
	void  SceneInstantiate::ResolveComponentPtr( const uint64_t _idOffset ) {
		// 		// Resolves components
		// 		for ( int componentPtrIndex = 0; componentPtrIndex < m_newComponentPtr.size(); componentPtrIndex++ )
		// 		{
		// 			ComponentIDPtr * ptr = m_newComponentPtr[componentPtrIndex];
		// 
		// 			auto it = m_remapTable.find( ptr->GetID().gameobjectID );
		// 			const uint64_t index = ( it != m_remapTable.end() ? it->second : ptr->GetID().gameobjectID );
		// 			Gameobject * gameobject =m_scene.FindGameobject( index );
		// 			if ( gameobject != nullptr )
		// 			{
		// 				Component * component = gameobject->GetComponent( ptr->GetID().componentID );
		// 				*ptr = ComponentIDPtr( component, IDPtrData( index, ptr->GetID().componentID ) );
		// 			}
		// 			else
		// 			{
		// 				Debug::Warning() << "Resolve component failed for gameobject index " << index << Debug::Endl();
		// 				*ptr = ComponentIDPtr();
		// 			}
		// 		}
	}

	//================================================================================================================================
	// Initialized gameobjects pointers are registered here to be resolved later when their target is fully loaded
	//================================================================================================================================
	void SceneInstantiate::RegisterUnresolvedGameobjectPtr( GameobjectPtr& _ptr )
	{
		assert( _ptr.GetId() > 0 );
		m_unresolvedGameobjectPtr.push_back( &_ptr );		
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::RegisterGameobjectPtr( GameobjectPtr& _gameobjectPtr )
	{
		assert( m_registeredGameobjectPtr.find( &_gameobjectPtr ) == m_registeredGameobjectPtr.end() );
		m_registeredGameobjectPtr.insert( &_gameobjectPtr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::UnregisterGameobjectPtr( GameobjectPtr& _gameobjectPtr )
	{
		assert( m_registeredGameobjectPtr.find( &_gameobjectPtr ) != m_registeredGameobjectPtr.end() );
		m_registeredGameobjectPtr.erase( &_gameobjectPtr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::OnComponentIDPtrCreate( ComponentIDPtr* _ptr )
	{
		// 		if ( _ptr->GetID().gameobjectID != 0 )@tmp
		// 		{
		// 			m_newComponentPtr.push_back( _ptr );
		// 		}
	}
}