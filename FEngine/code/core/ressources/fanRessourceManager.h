#pragma once

#include "core/ressources/fanRessource.h"

namespace ressource {
	class Ressource;
	class Mesh;

	class RessourceManager {
	public:
		template<typename RessourceType>
		static RessourcePtr<RessourceType> GetRessource( const std::string& _name );

		template<typename RessourceType> 
		static void RegisterRessource(const std::string& _path);

		static void DrawUI();

	private:
		static std::unordered_map< uint32_t, Ressource * > m_ressources;
		static std::unordered_map< uint32_t, Ressource * > m_defaultRessources;
	};

	//================================================================================================================================
	//================================================================================================================================
	template<typename RessourceType>
	RessourcePtr<RessourceType> RessourceManager::GetRessource(const std::string& _path) {
		static_assert((std::is_base_of<Ressource, RessourceType>::value));
		const uint32_t id = DSID(_path.c_str());

		auto defaultRessourceIt = m_defaultRessources.find(RessourceType::s_typeID);
		assert(defaultRessourceIt != m_defaultRessources.end());

		auto it = m_ressources.find(id);
		if (it != m_ressources.end()) {
			assert(dynamic_cast<RessourceType*>(it->second) != nullptr);


			return 	RessourcePtr<RessourceType>(
				static_cast<RessourceType*>(it->second), 
				static_cast<RessourceType*>(defaultRessourceIt->second) );
		}
		else {
			Mesh * mesh = new Mesh(_path);
			mesh->SetPath(_path);
			m_ressources[id] = mesh;
			return RessourcePtr<RessourceType>(mesh, static_cast<RessourceType*>(defaultRessourceIt->second));
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	template<typename RessourceType>
	void RessourceManager::RegisterRessource(const std::string& _path) {
		static_assert((std::is_base_of<Ressource, RessourceType>::value));
		assert(m_defaultRessources.find(RessourceType::s_typeID) == m_defaultRessources.end());
		

		RessourceType * defaultRessource = new RessourceType(_path);
		defaultRessource->Load();
		m_defaultRessources[RessourceType::s_typeID] = defaultRessource;
	}
}