#pragma once

#include "core/ecs/fanEcsConfig.h"
#include "core/ecs/fanComponentsTuple.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class EcsManager {
	public:

 		void OnGui();
	
		ecs::Entity CreateEntity() {
			if ( m_deletedEntities.empty() ) {
				return m_nextEntity++;
			} else {
				ecs::Entity entity = m_deletedEntities[ m_deletedEntities.size() - 1];
				m_deletedEntities.pop_back();
				return entity;
			}
		}

		void DeleteEntity( const ecs::Entity  _entity) {
			m_deletedEntities.push_back( _entity );
		}

	private:

		ComponentsTuple< ecs::Components > m_components;

		ecs::Entity m_nextEntity = 0;
		std::vector< ecs::Entity > m_deletedEntities;
 	};
}