#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	struct SceneNode;
	struct Scene;
	class Prefab;

	//================================================================================================================================
	// SceneInstantiate is also responsible for gameobject & components resolution and ID collisions.
	//================================================================================================================================
	class SceneInstantiate
	{
	public:
		SceneInstantiate( Scene& _scene );		

// 		void RegisterUnresolvedComponentPtr( ComponentPtrBase& _componentPtr );
// 		void RegisterUnresolvedGameobjectPtr( GameobjectPtr& _gameobjectPtr );
// 
// 		void RegisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );
// 		void UnregisterGameobjectPtr( GameobjectPtr& _gameobjectPtr );
// 		
// 		void RegisterComponentPtr(ComponentPtrBase& _componentPtr);
// 		void UnRegisterComponentPtr(ComponentPtrBase& _componentPtr);
// 
// 		void ResolveGameobjectPtr( const uint64_t _idOffset );
// 		void ResolveComponentPtr( const uint64_t _idOffset );
// 
// 		void UnregisterPointersForGameobject(const Gameobject* _gameobject);
// 		void UnregisterPointersForComponent( const Component* _component );

		void Clear();

	private:
		Scene& m_scene;

// 		std::vector< GameobjectPtr* >		m_unresolvedGameobjectPtr;
// 		std::set< GameobjectPtr* >			m_registeredGameobjectPtr;
// 
// 		std::vector< ComponentPtrBase* >	m_unresolvedComponentPtr;
// 		std::set< ComponentPtrBase* >		m_registeredComponentPtr;
	};
}