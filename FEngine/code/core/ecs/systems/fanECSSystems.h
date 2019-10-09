#pragma once

#include "core/ecs/fanECSConfig.h"
#include "core/ecs/EntityData.h"

namespace fan {
	//================================================================================================================================
// Signature
// Contains the component & tags typelists and their associated bitsets
// 'bitset' combines component & tags in the same bitset
//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ecsITag, _type >::value; };
	template< typename _type > struct IsComponent { static constexpr bool value = std::is_base_of< ecsIComponent, _type >::value; };
	template< typename _componentType > struct IndexOfComponent { static constexpr size_t value = meta::Find::List< _componentType, ecsComponents >::value; };
	template< typename _tagType >		struct IndexOfTag { static constexpr size_t value = meta::Find::List< _tagType, ecsTags >::value + ecsComponents::count; };

	template< typename... ComponentsAndTags >
	class ecsSignature {
	public:
		using componentsTypes = typename meta::Filter< IsComponent, ComponentsAndTags... >::type;
		using tagsTypes = typename meta::Filter< IsTag, ComponentsAndTags... >::type;
		static constexpr ecsBitset componentsBitset = ecsBitsetsComponents::GetList<componentsTypes>::value;
		static constexpr ecsBitset tagsBitset = ecsBitsetsTags::GetList<tagsTypes>::value;
		static constexpr ecsBitset bitset = componentsBitset | tagsBitset;
	};
	// testing
	static_assert( std::is_same< ecsSignature<ecsFakeTag, ecsMovement>::componentsTypes, meta::TypeList< ecsMovement>>::value );
	static_assert( std::is_same< ecsSignature<ecsMovement, ecsFakeTag>::tagsTypes, meta::TypeList< ecsFakeTag>>::value );

	//================================	
	// Declare your signatures here
	//================================	
	using ecsParticleSignature = ecsSignature< ecsTranform, ecsMovement, ecsParticle >;

	static constexpr ecsBitset tot1 = ecsParticleSignature::componentsBitset;

	//================================================================================================================================
	// System
	// Runs logic on entities matching a specific signature
	//================================================================================================================================
	template < typename... _signature> class ISystem;
	template < template <typename...> typename _typeList, typename... _args>
	class ISystem <_typeList<_args...> > {
	public:
		using signature = ecsSignature<_args...>;
	};

	//================================
	// ParticleSystem
	//================================
	class ParticleSystem : public ISystem<  ecsParticleSignature > {
	public:
		static void Run( float _delta, const std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsTranform > & _transforms,
			std::vector< ecsMovement > & _movements,
			std::vector< ecsParticle > & _particles ) 
		{
			for (int entity = 0; entity < _entitiesData.size(); entity++)
			{
				const ecsEntityData & data = _entitiesData[ entity ];

				if( data.IsAlive() && (data.bitset & ParticleSystem::signature::bitset) == ParticleSystem::signature::bitset ) {
					ecsTranform& transform = _transforms[data.components[IndexOfComponent<ecsTranform>::value]];
					ecsMovement& movement = _movements[data.components[IndexOfComponent<ecsMovement>::value]];
					ecsParticle& particle = _particles[data.components[IndexOfComponent<ecsParticle>::value]];
					Debug::Warning( "test" + std::to_string(_delta) );
					(void)_delta;(void )transform; (void)movement; (void)particle;
				}
			}
		}			
	};
}