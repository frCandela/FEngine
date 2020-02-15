#pragma once

#include "render/fanRenderPrecompiled.hpp"
#include "core/fanSingleton.hpp"
#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Device;
	class UIMesh;
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

		void Init( Device* _device );
		void Delete();

// 		bool IsModified() const { return m_modified; }
// 		void SetUnmodified() { m_modified = false; }


	private:
		Device* m_device;

		std::map< std::string, Prefab* > m_prefabs;
		std::set< UIMesh* >				 m_uiMeshList;

		void RegisterUIMesh( UIMesh* _mesh );

		// Vulkan data generation
		void OnGenerateUIMesh( UIMesh* _mesh );

		// Vulkan data generation
		void OnDeleteUIMesh( UIMesh* _mesh );

		std::string CleanPath( const std::string& _path );

		// Callbacks
		void OnResolvePrefabPtr( PrefabPtr* _ptr );
	};
}
