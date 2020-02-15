#include "render/fanResourceManager.hpp"
#include "render/core/fanTexture.hpp"
#include "render/core/fanBuffer.hpp"
#include "render/fanUIMesh.hpp"
#include "render/fanMesh.hpp"
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

		UIMesh::s_onGenerateVulkanData.Connect( &ResourceManager::OnGenerateUIMesh, this );
		UIMesh::s_onDeleteVulkanData.Connect( &ResourceManager::OnDeleteUIMesh, this );
	}

	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::Delete()
	{
		while ( !m_uiMeshList.empty() ) { delete* m_uiMeshList.begin(); }
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void ResourceManager::OnGenerateUIMesh( UIMesh* _mesh ) { _mesh->GenerateVulkanData( *m_device ); }

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
}