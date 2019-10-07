#pragma once

#include "core/ecs/fanEcsConfig.h"
#include "core/ecs/fanComponentsTuple.h"
#include "core/ecs/EntityData.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsManager {
	public:
		EcsManager();
	
		CEntity CreateEntity();
		void	DeleteEntity( const CEntity  _entity);

		void OnGui();

	private:
		ComponentsTuple< Components > m_components;
		std::vector<EntityData>		  m_entitiesData;

		CEntity m_lastEntity = 0;
		CEntity m_lastActiveEntity = 0;
 	};
}