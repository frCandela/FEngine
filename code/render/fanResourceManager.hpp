#pragma once

#include "render/fanRenderPrecompiled.hpp"
#include "core/fanSingleton.hpp"

namespace fan
{
	class Device;
	class UIMesh;
	class Component;
	class Gameobject;
	class Texture;
	class Mesh;
	class Prefab;
	struct IDPtrData;
	template< typename _ResourceType > class ResourcePtr;
	using GameobjectPtr = ResourcePtr<Gameobject>;
	using TexturePtr = ResourcePtr<Texture>;
	using MeshPtr = ResourcePtr<Mesh>;
	using PrefabPtr = ResourcePtr<Prefab>;

	//================================================================================================================================
	// Loads & references image and mesh the resources of the engine
	//================================================================================================================================
	class ResourceManager : public Singleton<ResourceManager> {
	public:
		friend class Singleton<ResourceManager>;

		void Init( Device* _device );
		void Delete();

		Mesh *		FindMesh	( const std::string& _path );
		Texture *	FindTexture	( const std::string& _path);

		Mesh *		LoadMesh	( const std::string& _path );		
		Texture *	LoadTexture	( const std::string& _path);

		bool IsModified() const { return m_modified; }
		void SetUnmodified() { m_modified = false; }

		const std::set< Mesh * >&			GetMeshList() const { return m_meshList; }
		const std::vector< Texture * > &	GetTextures() const { return m_textures; }

	private:
		Device * m_device;

		std::map< std::string, Prefab * > m_prefabs;
		std::set< Mesh * >		 m_meshList;
		std::set< UIMesh * >	 m_uiMeshList;
		std::vector< Texture * > m_textures;
		bool					 m_modified = false;

		void RegisterMesh( Mesh * _mesh );
		void RegisterUIMesh( UIMesh * _mesh );

		// Vulkan data generation
		void OnGenerateUIMesh( UIMesh * _mesh );
		void OnGenerateMesh( Mesh * _mesh );
		void OnGenerateTexture( Texture * _texture );

		// Vulkan data generation
		void OnDeleteUIMesh( UIMesh * _mesh );
		void OnDeleteMesh( Mesh * _mesh );
		void OnDeleteTexture( Texture * _texture );

		std::string CleanPath( const std::string& _path );

		// Callbacks
		void OnResolveTexturePtr( TexturePtr * _ptr );
		void OnResolveMeshPtr( MeshPtr * _ptr );
		void OnResolvePrefabPtr( PrefabPtr * _ptr );
	};
}
