#pragma once

#include "render/fanRenderPrecompiled.hpp"
#include "core/fanSingleton.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Device;
	class Component;
	class Gameobject;	
	class Prefab;
	struct IDPtrData;
	template< typename _ResourceType > class ResourcePtr;
	using GameobjectPtr = ResourcePtr<Gameobject>;
	using PrefabPtr = ResourcePtr<Prefab>;

	//================================================================================================================================
	// Loads & references image and mesh the resources of the engine
	//================================================================================================================================
	class ResourceManager : public Singleton<ResourceManager>
	{
	public:
		friend class Singleton<ResourceManager>;

	private:
		std::map< std::string, Prefab* > m_prefabs;
		std::string CleanPath( const std::string& _path );

		// Callbacks
		void OnResolvePrefabPtr( PrefabPtr* _ptr );
	};
}
