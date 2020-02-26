#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/fanGameobject.hpp"

namespace fan
{
	class Scene;

	//================================================================================================================================
	// holds a pointer to a gameobject of a specific  scene
	// can be initialized with a scene and a gameobject ID to be resolved later ( loading, copy/paste )
	//================================================================================================================================
	class GameobjectPtr : public ResourcePtr<Gameobject>
	{
	public:
		GameobjectPtr(  Gameobject* _gameobject = nullptr ) : ResourcePtr<Gameobject>( _gameobject ) {}
		~GameobjectPtr();

		void Init( Scene& _scene, uint64_t _gameobjectId );

		uint64_t GetId() const	 { return m_gameobjectId; }

		GameobjectPtr& operator=( Gameobject* _resource );

	private:
		uint64_t m_gameobjectId = 0;
		Scene * m_scene = nullptr;
	};	   

	//================================================================================================================================
	// holds a pointer to a prefab of the prefab resource manager
	// can be initialized with a prefab path to be resolved later
	//================================================================================================================================
	class PrefabPtr : public ResourcePtr<Prefab>
	{
	public:
		PrefabPtr( Prefab* _prefab = nullptr ) : ResourcePtr<Prefab>( _prefab ) {}

		void Init( const std::string _path ) { m_path = _path; }
		const std::string& GetPath() const { return m_path; }

		ResourcePtr& operator=( Prefab* _resource ) { SetResource( _resource ); return *this; }
	private:
		std::string m_path;
	};
}

//================================================================================================================================
// ImGui bindings
//================================================================================================================================
namespace ImGui
{
	bool FanGameobject( const char* _label, fan::GameobjectPtr& _ptr );
	bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
}
