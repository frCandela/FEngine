#pragma once

#include "core/meta/fanHelpers.h"
#include "core/meta/fanTypeList.h"
#include "core/ecs/fanComponentsBitset.h"
#include "core/ecs/fanComponentsTuple.h"
#include "renderer/util/fanColor.h"

namespace ecs {
	//================================================================================================================================
	// Components
	//================================================================================================================================	
	struct IComponent {};

	//================================
	struct Tranform : IComponent {
		btVector3		position;
		btQuaternion	rotation;
	};
	//================================
	struct Movement : IComponent {
		btVector3		speed;
	};
	//================================
	struct Color : IComponent {
		fan::Color		color;
	};
	//================================
	using Components = meta::TypeList<
		 Tranform
		,Movement
		,Color
	>;

	//================================================================================================================================
	// Tags
	//================================================================================================================================
	struct ITag {};
	struct Ally : ITag {};
	struct Ennemy : ITag {};

	using Tags = meta::TypeList<
		 Ally
		,Ennemy
	>;

	//================================================================================================================================
	// Bitsets & masks
	//================================================================================================================================
	using Entity = uint32_t;
	using Bitset = Bitset2::bitset2<Components::count>;
	using ComponentsBitset = meta::ComponentsBitSetImpl<Bitset, Components>;

	//================================================================================================================================
	// Signature
	//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ITag, _type >::value; };
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< IComponent, _type >::value; };
	template< typename... ComponentsAndTags > struct Signature {
		using components = typename meta::Filter< IsComponent, ComponentsAndTags... >::type;
		using tags		 = typename meta::Filter< IsTag,	   ComponentsAndTags... >::type;
	};

	// testing
	static_assert( std::is_same< Signature<Movement, Ally>::components, meta::TypeList< Movement>>::value );
	static_assert( std::is_same< Signature<Movement, Ally>::tags, meta::TypeList< Ally>>::value );

	//================================	
	using Dynamic    = Signature< Tranform, Movement >;
	using EnnemyShip = Signature< Tranform, Movement, Ennemy >; 
	using AllyShip   = Signature< Tranform, Movement, Ally >;
}

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

		ComponentsTuple< ecs::Components > _tuple;

		ecs::Entity m_nextEntity = 0;
		std::vector< ecs::Entity > m_deletedEntities;
	};
}