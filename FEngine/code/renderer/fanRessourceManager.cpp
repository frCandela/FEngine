#include "fanGlobalIncludes.h"

#include "renderer/fanRessourceManager.h"
#include "renderer/fanRenderer.h"
#include "renderer/core/fanTexture.h"
#include "renderer/core/fanBuffer.h"
#include "renderer/fanMesh.h"
#include "renderer/fanUIMesh.h"
#include "renderer/core/fanDevice.h"
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

		Mesh::s_onGenerateVulkanData.Connect	( &RessourceManager::OnGenerateMesh, this );
		UIMesh::s_onGenerateVulkanData.Connect	( &RessourceManager::OnGenerateUIMesh, this );		
		Texture::s_onGenerateVulkanData.Connect	( &RessourceManager::OnGenerateTexture, this );

		Mesh::s_onDeleteVulkanData.Connect( &RessourceManager::OnDeleteMesh,	this );
		UIMesh::s_onDeleteVulkanData.Connect( &RessourceManager::OnDeleteUIMesh,  this );
		Texture::s_onDeleteVulkanData.Connect( &RessourceManager::OnDeleteTexture, this );

		LoadMesh(GlobalValues::s_defaultMesh);
		LoadTexture(GlobalValues::s_defaultTexture);
		SetUnmodified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::Delete()
	{
		while ( ! m_textures.empty() ){	  delete *m_textures.begin(); }
		while ( ! m_meshList.empty() ){	  delete *m_meshList.begin(); }
		while ( ! m_uiMeshList.empty() ){ delete *m_uiMeshList.begin(); }
	}
	
	//================================================================================================================================
	//================================================================================================================================
	Mesh * RessourceManager::FindMesh( const std::string& _path )
	{
		for ( int meshIndex = 0; meshIndex < m_meshList.size(); meshIndex++ )
		{
			for ( Mesh* mesh : m_meshList )
			{
				if ( mesh->GetPath() == CleanPath( _path ) )
				{
					return mesh;
				}
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Texture * RessourceManager::FindTexture(const std::string& _path) {
		for (int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++) {
			const Texture * texture = m_textures[textureIndex];
			if (texture->GetPath() == _path) {
				return m_textures[textureIndex];
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	Prefab * RessourceManager::FindPrefab( const std::string& _path )
	{
		std::string path = CleanPath( _path );
		auto it = m_prefabs.find( path );
		return  it != m_prefabs.end() ? it->second : nullptr;
	}

	//================================================================================================================================
	// Load a mesh from a path, loads it and registers it
	//================================================================================================================================
	Mesh * RessourceManager::LoadMesh( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

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
	Texture * RessourceManager::LoadTexture( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		// Add
		Texture * texture = new Texture();
		if ( texture->LoadFromFile( _path ) == true )
		{
			texture->SetRenderID( static_cast<int>( m_textures.size() ) );
			m_textures.push_back( texture );
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
	Prefab *  RessourceManager::LoadPrefab( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

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
	void RessourceManager::OnGenerateMesh( Mesh * _mesh )			{ _mesh->GenerateVulkanData( *m_device );	}
	void RessourceManager::OnGenerateUIMesh( UIMesh * _mesh )		{ _mesh->GenerateVulkanData( *m_device );	}
	void RessourceManager::OnGenerateTexture( Texture * _texture )	{ _texture->GenerateVulkanData(*m_device);  }

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::OnDeleteUIMesh( UIMesh * _mesh )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		auto it = m_uiMeshList.find( _mesh );
		if( it != m_uiMeshList.end() ){	m_uiMeshList.erase( it ); }
		_mesh->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::OnDeleteMesh( Mesh * _mesh )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		auto it = m_meshList.find( _mesh );
		if ( it != m_meshList.end() ) { m_meshList.erase( it ); }
		_mesh->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::OnDeleteTexture( Texture * _texture )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		// Find the _texture if registered
		int foundIndex = -1;
		for ( int textureIndex = 0; textureIndex < m_textures.size() ; textureIndex++)
		{
			if ( m_textures[textureIndex] == _texture )
			{ 
				foundIndex = textureIndex;
				break; 
			}
		}

		// Remove the _texture if registered and remaps the remaining textures
		if ( foundIndex >= 0 )
		{
			m_textures.erase( m_textures.begin() + foundIndex );
			for ( int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++ )
			{
				 m_textures[textureIndex]->SetRenderID(textureIndex);
			}
		}

		_texture->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  RessourceManager::RegisterMesh(Mesh * _mesh) {
		assert( m_meshList.find(_mesh) == m_meshList.end() );
		m_meshList.insert( _mesh );
		_mesh->GenerateVulkanData( *m_device);
	}

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::RegisterUIMesh( UIMesh * _mesh )
	{		
		m_uiMeshList.insert( _mesh );
		_mesh->GenerateVulkanData( *m_device);
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