#include "scene/singletonComponents/fanSceneInstantiate.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/components/fanSceneNode.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	SceneInstantiate::SceneInstantiate( Scene& _scene ) : m_scene( _scene ) {}

	//================================================================================================================================
	//================================================================================================================================
	void SceneInstantiate::Clear()
	{
// 		// Clears gameobject ptr
// 		while ( ! m_registeredGameobjectPtr.empty() )
// 		{
// 			* ( * m_registeredGameobjectPtr.begin() ) = nullptr;
// 		}
// 		assert( m_registeredGameobjectPtr.empty() );
// 
// 		// Clears component ptr
// 		while (!m_registeredComponentPtr.empty())
// 		{
// 			*(*m_registeredComponentPtr.begin()) = nullptr;
// 		}
// 		assert(m_registeredComponentPtr.empty());
	}

// 	//================================================================================================================================
// 	// Resolves gameobject pointers ( find by id in the scene )
// 	// _idOffset adds an offset to the gameobject ids being resolved, it allow for easier instantiation of prefabs
// 	//================================================================================================================================
// 	void  SceneInstantiate::ResolveGameobjectPtr( const uint64_t _idOffset )
// 	{
// 		// Resolves gameobjects
// 		for ( int goPtrIndex = 0; goPtrIndex < m_unresolvedGameobjectPtr.size(); goPtrIndex++ )
// 		{			
// 			GameobjectPtr& ptr = *m_unresolvedGameobjectPtr[goPtrIndex];	
// 			const uint64_t id = ptr.GetId() + _idOffset;
// 			Gameobject * gameobject = m_scene.FindGameobject( id );	
// 			assert( ! ptr.IsValid() && ptr.GetId() != 0 );
// 
// 			if ( gameobject != nullptr )
// 			{
// 				ptr = gameobject;
// 			}
// 			else
// 			{
// 				Debug::Warning() << "Resolve gameobject failed for index " << id << Debug::Endl();
// 			}
// 		}
// 		m_unresolvedGameobjectPtr.clear();
// 	}
// 
// 	//================================================================================================================================
// 	// Resolves gameobject pointers
// 	//================================================================================================================================
// 	void  SceneInstantiate::ResolveComponentPtr( const uint64_t _idOffset ) {
// 
// 		// Resolves components
// 		for (int componentPtrIndex = 0; componentPtrIndex < m_unresolvedComponentPtr.size(); componentPtrIndex++)
// 		{
// 			ComponentPtrBase& ptr = *m_unresolvedComponentPtr[componentPtrIndex];
// 			const uint64_t gameobjectId = ptr.GetGameobjectId() + _idOffset;
// 			Gameobject* gameobject = m_scene.FindGameobject(gameobjectId);
// 			assert(!ptr.IsValid() && ptr.GetGameobjectId() != 0);
// 
// 			if (gameobject != nullptr)
// 			{
// 				Component* component = gameobject->GetComponent( ptr.GetComponentId() );
// 				if (component) {
// 					ptr = component;
// 				}				
// 			}
// 			else
// 			{
// 				Debug::Warning() << "Resolve component " << ptr.GetComponentId() << " failed for index " << gameobjectId << Debug::Endl();
// 			}
// 		}
// 		m_unresolvedComponentPtr.clear();
// 	}
// 
// 	//================================================================================================================================
// 	// Initialized gameobjects pointers are registered here to be resolved later when their target is fully loaded
// 	//================================================================================================================================
// 	void SceneInstantiate::RegisterUnresolvedGameobjectPtr( GameobjectPtr& _ptr )
// 	{
// 		assert( _ptr.GetId() > 0 );
// 		m_unresolvedGameobjectPtr.push_back( &_ptr );		
// 	}
// 
// 	//================================================================================================================================
// 	// Initialized component pointers are registered here to be resolved later when their target is fully loaded
// 	//================================================================================================================================
// 	void SceneInstantiate::RegisterUnresolvedComponentPtr( ComponentPtrBase& _componentPtr ) {
// 		assert( _componentPtr.GetGameobjectId() > 0 );
// 		m_unresolvedComponentPtr.push_back( &_componentPtr );
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SceneInstantiate::RegisterGameobjectPtr( GameobjectPtr& _gameobjectPtr )
// 	{
// 		assert( m_registeredGameobjectPtr.find( &_gameobjectPtr ) == m_registeredGameobjectPtr.end() );
// 		m_registeredGameobjectPtr.insert( &_gameobjectPtr );
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SceneInstantiate::UnregisterGameobjectPtr( GameobjectPtr& _gameobjectPtr )
// 	{
// 		assert( m_registeredGameobjectPtr.find( &_gameobjectPtr ) != m_registeredGameobjectPtr.end() );
// 		m_registeredGameobjectPtr.erase( &_gameobjectPtr );
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SceneInstantiate::RegisterComponentPtr(ComponentPtrBase& _componentPtr) {
// 		assert(m_registeredComponentPtr.find(&_componentPtr) == m_registeredComponentPtr.end());
// 		m_registeredComponentPtr.insert(&_componentPtr);
// 	}
// 
// 	//================================================================================================================================
// 	//================================================================================================================================
// 	void SceneInstantiate::UnRegisterComponentPtr(ComponentPtrBase& _componentPtr) {
// 		assert(m_registeredComponentPtr.find(&_componentPtr) != m_registeredComponentPtr.end());
// 		m_registeredComponentPtr.erase(&_componentPtr);
// 	}
// 
// 	//================================================================================================================================
// 	// Unregisters all gameobject pointers referencing a specific _gameobject
// 	//================================================================================================================================
// 	void SceneInstantiate::UnregisterPointersForGameobject( const Gameobject* _gameobject )
// 	{
// 		// Find pointers
// 		std::vector< GameobjectPtr* > pointers;
// 		for ( GameobjectPtr * ptr : m_registeredGameobjectPtr )
// 		{
// 			if ( **ptr  == _gameobject )
// 			{
// 				pointers.push_back( ptr );
// 			}			
// 		}	
// 
// 		// unregisters all
// 		for (GameobjectPtr * ptr : pointers)
// 		{
// 			*ptr = nullptr;
// 		}
// 	}
// 
// 	//================================================================================================================================
// 	// Unregisters all component pointers referencing a specific _component
// 	//================================================================================================================================
// 	void SceneInstantiate::UnregisterPointersForComponent( const Component* _component ) {
// 		// Find  pointers
// 		std::vector< ComponentPtrBase* > pointers;
// 		for (ComponentPtrBase* ptr : m_registeredComponentPtr)
// 		{
// 			if (**ptr == _component)
// 			{
// 				pointers.push_back( ptr );
// 			}
// 		}
// 
// 		// unregisters all
// 		for (ComponentPtrBase* ptr : pointers)
// 		{
// 			*ptr = nullptr;
// 		}
// 	}
}