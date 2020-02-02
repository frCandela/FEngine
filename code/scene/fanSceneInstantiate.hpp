 #pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanSceneRessourcePtr.hpp"
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
		SceneInstantiate( Scene & _scene );

		Gameobject *  InstantiateJson(	 const Json& _json, Gameobject * _parent );
		Gameobject *  InstanciatePrefab( const Prefab& _prefab, Gameobject * _parent );

	private:
		Scene& m_scene;

		std::vector< GameobjectPtr* >  m_newGameobjectPtr;
		std::vector< ComponentIDPtr* > m_newComponentPtr;
		std::map<uint64_t, uint64_t> m_remapTable;

		void OnSetIDFailed( uint64_t _id, Gameobject * _gameobject );
		void OnGameobjectPtrCreate( GameobjectPtr * _gameobjectPtr );
		void OnComponentIDPtrCreate( ComponentIDPtr * _ptr );
		void ResolvePointers();
	};
}