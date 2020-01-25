#pragma once

#include "core/fanTypeInfo.h"
#include "core/fanSignal.h"

namespace fan {
	//================================================================================================================================
	// Base class for ressources ( Mesh, textures etc.) 
	// Represent something with an path that needs to be loaded
	//================================================================================================================================
	class Ressource {
	public:				   
		virtual bool LoadFromFile( const std::string& _path );

		std::string GetPath() const { return m_path; }

	protected:
		std::string		m_path;
	};
}