#pragma once

#include "fanEcsTypes.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// A System is a static function Run() with no state that processes entities
	// System must implement GetSignature() and  Run(..) methods
	//==============================================================================================================================================================
	struct System
	{};
}

// class ecsParticleSunlightOcclusionSystem : public ISystem<  ecsPosition, ecsParticle, ecsSunlightParticleOcclusion >
// {
// public:
// 	static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 		, ComponentData< ecsPosition >& _positions
// 		, ComponentData< ecsParticle >& _particles
// 		, ComponentData< ecsSunlightParticleOcclusion >& _occlusion );
// };
// 
// 
// 
// //================================
// // Update bullet
// //================================
// class ecsUpdateBullet : public ISystem<  ecsGameobject, ecsBullet >
// {
// public:
// 	static void Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 		, ComponentData< ecsGameobject >& _gameobjects
// 		, ComponentData< ecsBullet >& _bullets
// 	);
// };
// 	//================================================================================================================================
// 	//================================================================================================================================
// void ecsParticleSunlightOcclusionSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 	, ComponentData< ecsPosition >& _positions
// 	, ComponentData< ecsParticle >&						/*_particles*/
// 	, ComponentData< ecsSunlightParticleOcclusion >&	/*_occlusion*/ )
// {
// 	// Get singleton components 
// 	ecsSunLightMesh_s& sunLight = _singletonComponents.GetComponent<ecsSunLightMesh_s>();
// 
// 	for( int entity = 0; entity < _count; entity++ )
// 	{
// 		ecsComponentsKey& key = _entitiesData[entity];
// 
// 		if( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 		{
// 			// get data
// 			btVector3& position = _positions.At( key ).position;
// 
// 			// raycast on the light mesh
// 			const btVector3 rayOrigin = btVector3( position[0], 1.f, position[2] );
// 			btVector3 outIntersection;
// 			bool isInsideSunlight = sunLight.mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );
// 			if( !isInsideSunlight )
// 			{
// 				key.Kill();
// 			}
// 		}
// 	}
// }
// 	//================================================================================================================================
// 	//================================================================================================================================
// void ecsUpdateBullet::Run( float _delta, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& /*_singletonComponents*/
// 	, ComponentData< ecsGameobject >& _gameobjects
// 	, ComponentData< ecsBullet >& _bullets )
// {
// 	// 		for ( int entity = 0; entity < _count; entity++ )
// 	// 		{
// 	// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 	// 
// 	// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 	// 			{
// 	// 				ecsBullet& bullet = _bullets.At( key );
// 	// 
// 	// 				bullet.durationLeft -= _delta;
// 	// 				if ( bullet.durationLeft <= 0.f )
// 	// 				{
// 	// 					Gameobject* gameobject = _gameobjects.At( key ).gameobject;
// 	// 					gameobject->GetScene().DeleteGameobject( gameobject );
// 	// 				}
// 	// 			}
// 	// 		}
// }
// }
