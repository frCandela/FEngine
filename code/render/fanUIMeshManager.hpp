#pragma once

#include <cassert>
#include <vector>

namespace fan
{
	class UIMesh;
	struct Device;

	//================================================================================================================================
	// Owns all the ui meshes of the engine
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