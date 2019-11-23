#include "fanGlobalIncludes.h"
#include "core/ressources/fanRessource.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	bool Ressource::LoadFromFile( const std::string& _path )
	{
		m_path = _path; 
		return true;
	};
}