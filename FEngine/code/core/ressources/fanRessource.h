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

		virtual bool	Load() { return true; }
		uint32_t		GetRessourceID() const { return m_ressourceID; }

		DECLARE_ABSTRACT_TYPE_INFO(Ressource, void )
	protected:

		Ressource() {}	
		virtual ~Ressource(){}
		void SetRessourceID(const uint32_t _ressourceID) { m_ressourceID = _ressourceID; }

	private:
		uint32_t	m_ressourceID;
	};
}