#pragma once

#include "core/fanTypeInfo.h"
#include "core/fanSignal.h"

namespace fan {
	//================================================================================================================================
	// Base class for ressources ( Mesh, textures etc.) 
	// Represent something with an ID that needs to be loaded
	//================================================================================================================================
	class Ressource {
	public:	

		virtual void	Load() {}
		uint32_t		GetRessourceID() const { return m_ressourceID; }

		DECLARE_ABSTRACT_TYPE_INFO(Ressource)
	protected:

		Ressource() {}		
		void SetRessourceID(const uint32_t _ressourceID) { m_ressourceID = _ressourceID; }

	private:
		uint32_t	m_ressourceID;
	};
}