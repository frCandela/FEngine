#include "fanGlobalIncludes.h"

#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanMesh.h"
#include "renderer/fanUIMesh.h"
#include "core/fanSignal.h"
#include "scene/fanGameobject.h"
#include "scene/fanPrefab.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::Init( Device* _device ) 
	{
		m_device = _device;
		m_textures.reserve(64);

		LoadMesh(GlobalValues::s_defaultMesh);
		LoadTexture(GlobalValues::s_defaultTexture);
		SetUnmodified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::Delete() 
	{
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			delete(m_textures[textureIndex]);
		} m_textures.clear();


		for ( Mesh * mesh : m_meshList ){
			delete mesh;
		} m_meshList.clear();

		for ( UIMesh * mesh : m_uiMeshList )
		{
			delete mesh;
		} m_uiMeshList.clear();
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture * RessourceManager::LoadTexture(const std::string _path) {
		if( _path.empty() ) { return nullptr; }

		// Dont add the texture if it already exists
		if (FindTexture(_path) != nullptr) {
			Debug::Get() << Debug::Severity::warning << "Texture already added: " << _path << Debug::Endl();
			return nullptr;
		}

		// Add
		Texture * texture = new Texture(*m_device);
		if (texture->LoadFromFile(_path) == true) {
			texture->SetRenderID(static_cast<int>(m_textures.size()));
			m_textures.push_back(texture);
			m_modified = true;
		}
		else
		{
			delete texture;
			texture = nullptr;
		}

		return texture;
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture * RessourceManager::FindTexture(const std::string _path) {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			const Texture * texture = m_textures[textureIndex];
			if (texture->GetPath() == _path) {
				return m_textures[textureIndex];
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	// Load a mesh from a path, loads it and registers it
	//================================================================================================================================
	Mesh * RessourceManager::LoadMesh( const std::string _path )
	{
		if( _path.empty() ) { return nullptr; }

		// Load
		Mesh * mesh = new Mesh();
		if ( mesh->LoadFromFile( CleanPath( _path ) ) )
		{
			RegisterMesh( mesh );
			return mesh;
		}
		delete mesh;
		return nullptr;

	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::OnLoadMesh( Mesh * _mesh ) {
		_mesh->GenerateVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::OnLoadUIMesh( UIMesh * _mesh )
	{
		_mesh->GenerateVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh * RessourceManager::FindMesh(const std::string _path) {
		for (int meshIndex = 0; meshIndex < m_meshList.size() ; meshIndex++) {
			for ( Mesh* mesh : m_meshList){
				if ( mesh->GetPath() ==  CleanPath(_path) ) {
					return mesh;
				}
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void  RessourceManager::RegisterMesh(Mesh * _mesh) {
		m_meshList.insert( _mesh );
		_mesh->GenerateVulkanData( *m_device);
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::RegisterUIMesh( UIMesh * _mesh )
	{
		m_uiMeshList.insert( _mesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab * RessourceManager::FindPrefab( const std::string& _path )
	{
		std::string path = CleanPath(_path);
		auto it = m_prefabs.find( path );
		return  it != m_prefabs.end() ? it->second : nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab *  RessourceManager::LoadPrefab( const std::string& _path )
	{
		if( _path.empty() ) { return nullptr; }

		// Load
		Prefab * prefab = new Prefab();
		if ( prefab->LoadFromFile( CleanPath( _path ) ) )
		{
			RegisterPrefab( prefab );
			return prefab;
		}
		delete prefab;
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::RegisterPrefab( Prefab * _prefab )
	{
		std::string path = CleanPath( _prefab->GetPath() );
		assert( m_prefabs.find(path) == m_prefabs.end() );
		m_prefabs[ path ] = _prefab;
	}

	//================================================================================================================================
	// the / is dead, long live the \ 
	//================================================================================================================================
	std::string RessourceManager::CleanPath( const std::string& _path )
	{
		std::fs::path path = _path;
		path.make_preferred();

		return path.string();
	}
}