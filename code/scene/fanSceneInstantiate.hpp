#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanComponentPtr.hpp"

namespace fan
{
	class Gameobject;
	class Component;

	class Scene;
	class Prefab;

	//================================================================================================================================
	// Allow gameobject instantiation from json data (copy/paste, prefabs)
	// SceneInstantiate is also responsible for gameobject & components resolution and ID collisions.
	//================================================================================================================================
	class SceneInstantiate
	{
	public:
		SceneInstantiate( Scene& _scene );		
		Gameobject* InstanciatePrefab( const Prefab& _prefab, Gameobject& _parent );

		void RegisterUnresolvedComponentPtr( ComponentPtrBase& _componentPtr );
		void RegisterUnresolvedGameobjectPtr( GameobjectPtr& _gameobjectPtr );

		void RegisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );
		void UnregisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );
		
		void RegisterComponentPtr(ComponentPtrBase& _componentPtr);
		void UnRegisterComponentPtr(ComponentPtrBase& _componentPtr);

		void ResolveGameobjectPtr( const uint64_t _idOffset );
		void ResolveComponentPtr( const uint64_t _idOffset );

		void UnregisterPointersForGameobject(const Gameobject* _gameobject);
		void UnregisterPointersForComponent( const Component* _component );

		void Clear();

	private:
		Scene& m_scene;

		std::vector< GameobjectPtr* >		m_unresolvedGameobjectPtr;
		std::set< GameobjectPtr* >			m_registeredGameobjectPtr;

		std::vector< ComponentPtrBase* >	m_unresolvedComponentPtr;
		std::set< ComponentPtrBase* >		m_registeredComponentPtr;

		Gameobject* InstantiateJson( const Json& _json, Gameobject& _parent );
	};
}