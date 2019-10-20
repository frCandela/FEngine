#pragma once

#include "ecs/fanECSConfig.h"
#include "ecs/fanECSEntityData.h"

namespace fan {
//================================================================================================================================
// Signature
// Contains the component & tags typelists and their associated bitsets
// 'bitset' combines component & tags in the same bitset
//================================================================================================================================
	template< typename _type > struct IsTag { static constexpr bool value = std::is_base_of< ecsITag, _type >::value; };
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
	using ecsParticleSignature			= ecsSignature< ecsPosition, ecsRotation, ecsMovement, ecsParticle >;
	using ecsPlanetSignature			= ecsSignature< ecsTranform, ecsPlanet >;
	using ecsRigidbodySignature			= ecsSignature< ecsTranform, ecsMotionState, ecsRigidbody >;
	using ecsAABBUpdateFromHull			= ecsSignature< ecsTranform, ecsScaling, ecsAABB, ecsFlags, ecsConvexHull >;
	using ecsAABBUpdateFromTransform	= ecsSignature< ecsTranform, ecsAABB, ecsFlags >;

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
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsPosition > & _positions,
			std::vector< ecsRotation > & _rotations,
			std::vector< ecsMovement > & _movements,
			std::vector< ecsParticle > & _particles );
	};

	//================================
	// PlanetsSystem
	//================================
	class PlanetsSystem : public ISystem<  ecsPlanetSignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsTranform > & _transforms,
			std::vector< ecsPlanet > & _planets );
	};

	//================================
	// Rigidbody transform update
	//================================
	class SynchTransToRbSystem : public ISystem<  ecsRigidbodySignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			  std::vector< ecsTranform > & _transforms
			, std::vector< ecsMotionState > & _motionStates
			, std::vector< ecsRigidbody > & _rigidbodies );
	};

	//================================
	// Rigidbody transform update
	//================================
	class SynchRbToTransSystem : public ISystem<  ecsRigidbodySignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsTranform > & _transforms
			, std::vector< ecsMotionState > & _motionStates
			, std::vector< ecsRigidbody > & _rigidbodies );
	};

	//================================
	// Update AABB from convex hull
	//================================
	class UpdateAABBFromHull : public ISystem<  ecsAABBUpdateFromHull > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsTranform > &	_transforms
			, std::vector< ecsScaling > &	_scales
			, std::vector< ecsAABB > &		_aabbs
			, std::vector< ecsFlags > &		_flags 
			, std::vector< ecsConvexHull > & _hulls
		);
	};

	//================================
	// Update AABB from transform
	//================================
	class UpdateAABBFromTransform : public ISystem<  ecsAABBUpdateFromTransform > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsEntityData >& _entitiesData,
			std::vector< ecsTranform > &	_transforms
			, std::vector< ecsAABB > &		_aabbs
			, std::vector< ecsFlags > &		_flags
		);
	};
}