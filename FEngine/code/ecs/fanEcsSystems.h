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

	//================================================================================================================================
	// System
	// Runs logic on entities matching a specific signature
	//================================================================================================================================
	template < typename... _args>
	class ISystem {
	public:
		using signature = ecsSignature<_args...>;
	};

	//================================
	// ParticleSystem
	//================================
	class ecsParticleSystem : public ISystem<  ecsPosition, ecsRotation, ecsMovement, ecsParticle > {
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
	class ecsPlanetsSystem : public ISystem<  ecsGameobject, ecsTranform, ecsPlanet, ecsFlags > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			 ComponentData< ecsGameobject > & _gameobjects
			,ComponentData< ecsTranform > & _transforms
			,ComponentData< ecsPlanet > &	_planets 
			,ComponentData< ecsFlags > &	_flags );
	};

	//================================
	// PlanetsSystem
	//================================
	struct Vertex;
	class Mesh;
	class ecsSolarEruptionMeshSystem : public ISystem< ecsPlanet, ecsTranform, ecsScaling >
	{
	private:
		//================================================================
		//================================================================
		struct OrientedSegment
		{
			enum OpenSide { RIGHT = 1, LEFT = 2, BOTH = RIGHT | LEFT };

			btVector3 direction;
			OpenSide  openSide;
			float norm;
		};

	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData
			, ComponentData< ecsPlanet > &	_planets
			, ComponentData< ecsTranform > & _transforms 
			, ComponentData< ecsScaling > & _scaling
		);

		// This should be in a "singleton component"
		static Mesh* s_mesh;
		static float s_subAngle;
		static float s_radius;
		static bool	 s_debugDraw;

	private:
		static void AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 );


	};

	//================================
	// Rigidbody transform update
	//================================
	class ecsSynchRbSystem : public ISystem<  ecsTranform, ecsMotionState, ecsRigidbody > {
	public:
		static void SynchTransToRbSystem( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			 ComponentData< ecsTranform > & _transforms
			,ComponentData< ecsMotionState > & _motionStates
			,ComponentData< ecsRigidbody > & _rigidbodies );

		static void SynchRbToTransSystem( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			ComponentData< ecsTranform > &	_transforms
			, ComponentData< ecsMotionState > & _motionStates
			, ComponentData< ecsRigidbody > &	_rigidbodies );
	};

	//================================
	// Update AABB from convex hull
	//================================
	class ecsUpdateAABBFromHull : public ISystem<  ecsTranform, ecsScaling, ecsAABB, ecsFlags, ecsMesh >
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
	class ecsUpdateAABBFromRigidbody : public ISystem<  ecsAABB, ecsRigidbody, ecsFlags > {
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
	class ecsUpdateAABBFromTransform : public ISystem<  ecsTranform, ecsAABB, ecsFlags > {
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			  ComponentData< ecsTranform > &	_transforms
			, ComponentData< ecsAABB >     &	_aabbs
			, ComponentData< ecsFlags >    &	_flags
		);
	};

	//================================
	// Update bullet
	//================================
	class ecsUpdateBullet : public ISystem<  ecsGameobject, ecsBullet >
	{
	public:
		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData,
			ComponentData< ecsGameobject > &	_gameobjects
			, ComponentData< ecsBullet >     &	_bullets
		);
	};
}