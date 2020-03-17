#pragma once

#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	//================================
	// ParticleSystem
	//================================
// 	class ecsParticleSunlightOcclusionSystem : public ISystem<  ecsPosition, ecsParticle, ecsSunlightParticleOcclusion >
// 	{
// 	public:
// 		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 						 , ComponentData< ecsPosition >& _positions
// 						 , ComponentData< ecsParticle >& _particles
// 						 , ComponentData< ecsSunlightParticleOcclusion >& _occlusion );
// 	};
// 
// 	//================================
// 	// PlanetsSystem
// 	//================================
// 	class ecsPlanetsSystem : public ISystem<  ecsGameobject, ecsTranform, ecsPlanet, ecsFlags >
// 	{
// 	public:
// 		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 						 , ComponentData< ecsGameobject >& _gameobjects
// 						 , ComponentData< ecsTranform >& _transforms
// 						 , ComponentData< ecsPlanet >& _planets
// 						 , ComponentData< ecsFlags >& _flags );
// 	};
// 
// 	//================================
// 	// PlanetsSystem
// 	//================================
// 	struct Vertex;
// 	class Mesh;
// 	class ecsSolarEruptionMeshSystem : public ISystem< ecsPlanet, ecsTranform, ecsScaling >
// 	{
// 	private:
// 		//================================================================
// 		//================================================================
// 		struct OrientedSegment
// 		{
// 			enum OpenSide { RIGHT = 1, LEFT = 2, BOTH = RIGHT | LEFT };
// 
// 			btVector3 direction;
// 			OpenSide  openSide;
// 			float norm;
// 		};
// 
// 	public:
// 		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 						 , ComponentData< ecsPlanet >& _planets
// 						 , ComponentData< ecsTranform >& _transforms
// 						 , ComponentData< ecsScaling >& _scaling
// 		);
// 	};
// 	//================================
// 	// Update bullet
// 	//================================
// 	class ecsUpdateBullet : public ISystem<  ecsGameobject, ecsBullet >
// 	{
// 	public:
// 		static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 						 , ComponentData< ecsGameobject >& _gameobjects
// 						 , ComponentData< ecsBullet >& _bullets
// 		);
// 	};
}