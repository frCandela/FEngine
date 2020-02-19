#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanSceneResourcePtr.hpp"
#include "scene/fanComponentPtr.hpp"

namespace fan
{
	class Gameobject;
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

		void RegisterUnresolvedGameobjectPtr( GameobjectPtr& _gameobjectPtr );
		void RegisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );
		void UnregisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );

		void ResolveGameobjectPtr( const uint64_t _idOffset );
		void ResolveComponentPtr( const uint64_t _idOffset );

		void Clear();

	private:
		Scene& m_scene;

		std::vector< GameobjectPtr* >   m_unresolvedGameobjectPtr;
		std::set< GameobjectPtr* >		m_registeredGameobjectPtr;
		std::vector< ComponentIDPtr* >  m_newComponentPtr;
		
		Gameobject* InstantiateJson( const Json& _json, Gameobject& _parent );
		void OnComponentIDPtrCreate( ComponentIDPtr* _ptr );
		
	};
}