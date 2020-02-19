#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"
#include "scene/fanPrefab.hpp"
#include "scene/fanGameobject.hpp"

namespace fan
{
	class Scene;

	//================================================================================================================================
	//================================================================================================================================
	class GameobjectPtr : public ResourcePtr<Gameobject>
	{
	public:
		GameobjectPtr(  Gameobject* _gameobject = nullptr ) : ResourcePtr<Gameobject>( _gameobject ) {}

		void Init( Scene& _scene, uint64_t _id );

		uint64_t GetId() const	 { return m_id; }

		GameobjectPtr& operator=( Gameobject* _resource );

	private:
		uint64_t m_id = 0;
		Scene * m_scene = nullptr;
	};	   

	//================================================================================================================================
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
