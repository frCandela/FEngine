#pragma once

#include "render/fanRenderPrecompiled.hpp"

namespace fan
{
	class UIMesh;
	class Device;

	//================================================================================================================================
	// Owns all the meshes of the engine
	// load, find, resolve pointers, delete
	//================================================================================================================================
	class UIMeshManager
	{
	public:
		void Init( Device& _device );
		void Clear();

		Device& GetDevice() const { assert( m_device != nullptr ); return *m_device;  }

	private:
		Device* m_device = nullptr;
		std::vector< UIMesh* > m_meshList;
	};
}