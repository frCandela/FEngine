#pragma once

#include "core/fanSingleton.h"

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
	template< typename _RessourceType, typename _IDType > class RessourcePtr;
	using GameobjectPtr = RessourcePtr<Gameobject, uint64_t>;
	using TexturePtr = RessourcePtr<Texture, std::string>;
	using MeshPtr = RessourcePtr<Mesh, std::string>;
	using PrefabPtr = RessourcePtr<Prefab, std::string>;

	//================================================================================================================================
	// Loads & references all the ressources of the engine
	//================================================================================================================================
	class RessourceManager : public Singleton<RessourceManager> {
	public:
		friend class Singleton<RessourceManager>;

		void Init( Device* _device );
		void Delete();

		Mesh *		FindMesh	( const std::string& _path );
		Texture *	FindTexture	( const std::string& _path);
		Prefab *	FindPrefab	( const std::string& _path );

		Mesh *		LoadMesh	( const std::string& _path );		
		Texture *	LoadTexture	( const std::string& _path);
		Prefab *	LoadPrefab	( const std::string& _path );

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

		void RegisterPrefab( Prefab * _prefab );
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
