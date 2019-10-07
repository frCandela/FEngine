#pragma once

#include "core/ecs/fanEcsConfig.h"
#include "core/ecs/fanComponentsTuple.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsManager {
	public:

 		void OnGui();
	
		CEntity CreateEntity() {
			if ( m_deletedEntities.empty() ) {
				return m_nextEntity++;
			} else {
				CEntity entity = m_deletedEntities[ m_deletedEntities.size() - 1];
				m_deletedEntities.pop_back();
				return entity;
			}
		}

		void DeleteEntity( const CEntity  _entity) {
			m_deletedEntities.push_back( _entity );
		}

	private:

		ComponentsTuple< Components > m_components;

		CEntity m_nextEntity = 0;
		std::vector< CEntity > m_deletedEntities;
 	};
}