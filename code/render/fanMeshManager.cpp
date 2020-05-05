#include "render/fanMeshManager.hpp"

#include <filesystem>
#include "render/fanRenderResourcePtr.hpp"
#include "render/fanMesh.hpp"
#include "render/fanRenderGlobal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::Init( Device& _device )
	{
		m_device = &_device;
		ResourcePtr< Mesh >::s_onResolve.Connect( &MeshManager::ResolvePtr, this );
		GetMesh( RenderGlobal::s_defaultMesh );
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::Clear()
	{
		while ( !m_meshList.empty() ) 
		{ 
			delete m_meshList.begin()->second; 
			m_meshList.erase( m_meshList.begin() );
		}
		m_device = nullptr;
	}

	//================================================================================================================================
	//================================================================================================================================
	void MeshManager::ResolvePtr( ResourcePtr< Mesh >& _resourcePtr )
	{
		assert( ! _resourcePtr.IsValid() );

		MeshPtr& meshPtr = static_cast< MeshPtr& >( _resourcePtr );
		Mesh * mesh = GetMesh( meshPtr.GetPath() );

		if ( mesh != nullptr )
		{
			meshPtr = mesh;
		}
	}

	//================================================================================================================================
	// returns a mesh at a specific path
	// first tries to find it, else loads it from a path and registers it
	//================================================================================================================================
	Mesh* MeshManager::GetMesh( const std::string& _path )
	{
		if ( _path.empty() ) { return nullptr; }

		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();

		Mesh* mesh = FindMesh( cleanPath );
		if ( mesh != nullptr )
		{
			return mesh;
		}
		else
		{
			// Load
			mesh = new Mesh();			
			if ( mesh->LoadFromFile( cleanPath ) )
			{	
				m_meshList[cleanPath] = mesh;
				return mesh;
			}
			delete mesh;
			return nullptr;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Mesh* MeshManager::FindMesh( const std::string& _path )
	{
		const std::string cleanPath = std::filesystem::path( _path ).make_preferred().string();
		auto it = m_meshList.find( cleanPath );
		return it == m_meshList.end() ? nullptr : it->second;
	}
}