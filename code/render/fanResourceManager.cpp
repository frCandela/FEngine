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
	// the / is dead, long live the \ 
	//================================================================================================================================
	std::string ResourceManager::CleanPath( const std::string& _path )
	{
		std::filesystem::path path = _path;
		path.make_preferred();

		return path.string();
	}
}