#pragma once

#include "render/fanRenderPrecompiled.hpp"

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
	class Mesh;
	class Device;

	//================================================================================================================================
	//================================================================================================================================
	class MeshManager
	{
	public:
		void Init( Device& _device );
		void Clear();

		void	ResolvePtr( ResourcePtr< Mesh >& _resourcePtr );
		Device* GetDevice() const { return m_device;  }
		Mesh*	LoadMesh( const std::string& _path );

		const std::map< std::string, Mesh* >& GetList() const { return m_meshList; }

	private:
		Device* m_device = nullptr;
		std::map< std::string, Mesh* > m_meshList;

		Mesh* FindMesh( const std::string& _path );
	};
}