#pragma once

#include "core/fanCorePrecompiled.hpp"
#include "core/fanTypeInfo.hpp"
#include "core/fanSignal.hpp"

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