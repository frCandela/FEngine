#pragma once

#include "ecs/fanECSConfig.h"
#include "ecs/fanEcsComponentsKey.h"
#include "ecs/fanEcsComponentsTuple.h"

namespace fan {
//================================================================================================================================
// Signature
// Contains the component & tags typelists and their associated bitsets
// 'bitset' combines component & tags in the same bitset
//================================================================================================================================
	template< typename _type >			struct IsTag { static constexpr bool value = std::is_base_of< ecsITag, _type >::value; };
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
	using ecsPlanetSignature			= ecsSignature< ecsTranform, ecsPlanet, ecsFlags >;
	using ecsRigidbodySignature			= ecsSignature< ecsTranform, ecsMotionState, ecsRigidbody >;
	using ecsAABBUpdateFromHull			= ecsSignature< ecsTranform, ecsScaling, ecsAABB, ecsFlags, ecsMesh >;
	using ecsAABBUpdateFromTransform	= ecsSignature< ecsTranform, ecsAABB, ecsFlags >;
	using ecsAABBUpdateFromRigidbody    = ecsSignature< ecsAABB, ecsRigidbody, ecsFlags >;

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
	class ecsParticleSystem : public ISystem<  ecsParticleSignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			ComponentData< ecsPosition > & _positions,
			ComponentData< ecsRotation > & _rotations,
			ComponentData< ecsMovement > & _movements,
			ComponentData< ecsParticle > & _particles );

		static btVector3 s_cameraPosition;
	};

	//================================
	// PlanetsSystem
	//================================
	class ecsPlanetsSystem : public ISystem<  ecsPlanetSignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			 ComponentData< ecsTranform > & _transforms
			,ComponentData< ecsPlanet > &	_planets 
			,ComponentData< ecsFlags > &	_flags );
	};

	//================================
	// Rigidbody transform update
	//================================
	class ecsSynchTransToRbSystem : public ISystem<  ecsRigidbodySignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			 ComponentData< ecsTranform > & _transforms
			,ComponentData< ecsMotionState > & _motionStates
			,ComponentData< ecsRigidbody > & _rigidbodies );
	};

	//================================
	// Rigidbody transform update
	//================================
	class ecsSynchRbToTransSystem : public ISystem<  ecsRigidbodySignature > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			  ComponentData< ecsTranform > &	_transforms
			, ComponentData< ecsMotionState > & _motionStates
			, ComponentData< ecsRigidbody > &	_rigidbodies );
	};

	//================================
	// Update AABB from convex hull
	//================================
	class ecsUpdateAABBFromHull : public ISystem<  ecsAABBUpdateFromHull >
	{
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			ComponentData< ecsTranform > &		_transforms
			, ComponentData< ecsScaling > &		_scales
			, ComponentData< ecsAABB > &		_aabbs
			, ComponentData< ecsFlags > &		_flags
			, ComponentData< ecsMesh >&			_mesh
		);
	};

	//================================
	// Update AABB from rigidbody
	//================================
	class ecsUpdateAABBFromRigidbody : public ISystem<  ecsAABBUpdateFromRigidbody > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,

			 ComponentData< ecsAABB > &			_aabbs
			,ComponentData< ecsRigidbody > &	_rigidbodies
			, ComponentData< ecsFlags >    &	_flags
		);
	};

	//================================
	// Update AABB from transform
	//================================
	class ecsUpdateAABBFromTransform : public ISystem<  ecsAABBUpdateFromTransform > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			  ComponentData< ecsTranform > &	_transforms
			, ComponentData< ecsAABB >     &	_aabbs
			, ComponentData< ecsFlags >    &	_flags
		);
	};
}