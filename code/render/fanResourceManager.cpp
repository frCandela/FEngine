#include "render/fanResourceManager.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanMesh.hpp"
#include "render/fanUIMesh.hpp"
#include "render/core/fanDevice.hpp"
#include "render/fanRenderGlobal.hpp"
#include "render/fanRenderResourcePtr.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::Init( Device* _device )
	{
		m_device = _device;
		m_textures.reserve( 64 );

		Mesh::s_onGenerateVulkanData.Connect( &ResourceManager::OnGenerateMesh, this );
		UIMesh::s_onGenerateVulkanData.Connect( &ResourceManager::OnGenerateUIMesh, this );
		Texture::s_onGenerateVulkanData.Connect( &ResourceManager::OnGenerateTexture, this );

		Mesh::s_onDeleteVulkanData.Connect( &ResourceManager::OnDeleteMesh, this );
		UIMesh::s_onDeleteVulkanData.Connect( &ResourceManager::OnDeleteUIMesh, this );
		Texture::s_onDeleteVulkanData.Connect( &ResourceManager::OnDeleteTexture, this );

		// 		TexturePtr::s_onInit.Connect ( &ResourceManager::OnResolveTexturePtr,this );
		// 		MeshPtr::s_onInit.Connect	( &ResourceManager::OnResolveMeshPtr, this );

		LoadMesh( RenderGlobal::s_defaultMesh );
		LoadTexture( RenderGlobal::s_defaultTexture );
		LoadTexture( RenderGlobal::s_whiteTexture );
		SetUnmodified();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::Delete()
	{
		while ( !m_textures.empty() ) { delete* m_textures.begin(); }
		while ( !m_meshList.empty() ) { delete* m_meshList.begin(); }
		while ( !m_uiMeshList.empty() ) { delete* m_uiMeshList.begin(); }
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh* ResourceManager::FindMesh( const std::string& _path )
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
	Texture* ResourceManager::FindTexture( const std::string& _path )
	{
		for ( int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++ )
		{
			const Texture* texture = m_textures[ textureIndex ];
			if ( texture->GetPath() == _path )
			{
				return m_textures[ textureIndex ];
			}
		}
		return nullptr;
	}

	//================================================================================================================================
	// Load a mesh from a path, loads it and registers it
	//================================================================================================================================
	Mesh* ResourceManager::LoadMesh( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		// Load
		Mesh* mesh = new Mesh();
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
	Texture* ResourceManager::LoadTexture( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		// Add
		Texture* texture = new Texture();
		if ( texture->LoadFromFile( _path ) == true )
		{
			texture->SetRenderID( static_cast< int >( m_textures.size() ) );
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
	void ResourceManager::OnGenerateMesh( Mesh* _mesh ) { _mesh->GenerateVulkanData( *m_device ); }
	void ResourceManager::OnGenerateUIMesh( UIMesh* _mesh ) { _mesh->GenerateVulkanData( *m_device ); }
	void ResourceManager::OnGenerateTexture( Texture* _texture ) { _texture->GenerateVulkanData( *m_device ); }

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnDeleteUIMesh( UIMesh* _mesh )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		auto it = m_uiMeshList.find( _mesh );
		if ( it != m_uiMeshList.end() ) { m_uiMeshList.erase( it ); }
		_mesh->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnDeleteMesh( Mesh* _mesh )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		auto it = m_meshList.find( _mesh );
		if ( it != m_meshList.end() ) { m_meshList.erase( it ); }
		_mesh->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnDeleteTexture( Texture* _texture )
	{
		vkDeviceWaitIdle( m_device->vkDevice );
		Debug::Highlight( "Renderer idle" );

		// Find the _texture if registered
		int foundIndex = -1;
		for ( int textureIndex = 0; textureIndex < m_textures.size(); textureIndex++ )
		{
			if ( m_textures[ textureIndex ] == _texture )
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
				m_textures[ textureIndex ]->SetRenderID( textureIndex );
			}
		}

		_texture->DeleteVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void  ResourceManager::RegisterMesh( Mesh* _mesh )
	{
		assert( m_meshList.find( _mesh ) == m_meshList.end() );
		m_meshList.insert( _mesh );
		_mesh->GenerateVulkanData( *m_device );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::RegisterUIMesh( UIMesh* _mesh )
	{
		m_uiMeshList.insert( _mesh );
		_mesh->GenerateVulkanData( *m_device );
	}

	//================================================================================================================================
	// the / is dead, long live the \ 
	//================================================================================================================================
	std::string ResourceManager::CleanPath( const std::string& _path )
	{
		std::filesystem::path path = _path;
		path.make_preferred();

		return path.string();
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnResolveTexturePtr( TexturePtr* _ptr )
	{
		// 		Texture * texture = ResourceManager::Get().FindTexture( _ptr->GetID() );@tmp
		// 		if ( texture == nullptr )
		// 		{
		// 			texture = ResourceManager::Get().LoadTexture( _ptr->GetID() );
		// 		}
		// 		if ( texture )
		// 		{
		// 			*_ptr = TexturePtr( texture, texture->GetPath() );
		// 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnResolveMeshPtr( MeshPtr* _ptr )
	{
		// 		Mesh * mesh = ResourceManager::Get().FindMesh( _ptr->GetID() );@tmp
		// 		if ( mesh == nullptr )
		// 		{
		// 			mesh = ResourceManager::Get().LoadMesh( _ptr->GetID() );
		// 		}
		// 		if ( mesh )
		// 		{
		// 			*_ptr = MeshPtr( mesh, mesh->GetPath() );
		// 		}
	}
}