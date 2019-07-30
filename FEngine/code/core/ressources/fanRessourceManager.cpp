#include "fanIncludes.h"

#include "core/ressources/fanRessourceManager.h"
#include "core/ressources/fanMesh.h"

namespace ressource	 {

	std::unordered_map< uint32_t, Ressource * > RessourceManager::m_ressources = {};
	std::unordered_map< uint32_t, Ressource * > RessourceManager::m_defaultRessources = {};

	//================================================================================================================================
	//================================================================================================================================
	void RessourceManager::DrawUI() {
		ImGui::Text("ressources: ");
		for (auto& it : m_ressources) {
			std::stringstream ss;
			ss << it.first;
			ImGui::Text(ss.str().c_str());
		}
	}
}