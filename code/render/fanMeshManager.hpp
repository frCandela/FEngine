#pragma once

#include "render/fanRenderPrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Mesh;
	class Device;

	//================================================================================================================================
	// Owns all the meshes of the engine
	// load, find, resolve pointers, delete
	//================================================================================================================================
	class MeshManager
	{
	public:
		void Init( Device& _device );
		void Clear();

		bool IsCleared() { return m_device == nullptr; } //@hack ressources gpu buffers deletion problem

		void	ResolvePtr( ResourcePtr< Mesh >& _resourcePtr );
		Device& GetDevice() const { assert( m_device != nullptr ); return *m_device;  }
		Mesh*	GetMesh( const std::string& _path );

		const std::map< std::string, Mesh* >& GetList() const { return m_meshList; }

	private:
		Device* m_device = nullptr;
		std::map< std::string, Mesh* > m_meshList;

		Mesh* FindMesh( const std::string& _path );
	};
}