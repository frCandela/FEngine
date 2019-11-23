#pragma once

#include "core/fanSingleton.h"

namespace fan
{
	class Mesh;
	class UIMesh;
	class Texture;
	class Prefab;

	class Device;
	class Buffer;

	//================================================================================================================================
	// Loads & references all the ressources of the engine
	//================================================================================================================================
	class RessourceManager : public Singleton<RessourceManager> {
	public:
		friend class Singleton<RessourceManager>;

		void Init( Device* _device );
		void Delete();

		// Mesh
		Mesh *	FindMesh( const std::string _path );
		Mesh *	LoadMesh( const std::string _path );
		void	OnLoadMesh( Mesh * _mesh );

		// UI Mesh
		void	OnLoadUIMesh( UIMesh * _mesh );		

		// Texture
		Texture *	FindTexture(const std::string _path);
		Texture *	LoadTexture(const std::string _path);

		// Prefab
		Prefab *	FindPrefab( const std::string& _path );
		Prefab *	LoadPrefab( const std::string& _path );

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

		void		RegisterPrefab( Prefab * _prefab );
		void		RegisterMesh(	Mesh * _mesh );
		void		RegisterUIMesh( UIMesh * _mesh );
		std::string CleanPath( const std::string& _path );
	};
}
