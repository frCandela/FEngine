#pragma  once

#include "scene/fanScenePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct Particle : public Component
	{
		DECLARE_COMPONENT( Particle )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( EcsWorld& _world, Component& _component );

		Color		color;
		glm::vec3	position;
		glm::vec3	speed;
		float		durationLeft;
	};
	static constexpr size_t sizeof_particle = sizeof( Particle );
}

//@old 04/01/2020 maybe usefull sometime ?
// 	void ecsParticleSunlightOcclusionSystem::Run( float /*_delta*/, const size_t _count, std::vector< ecsComponentsKey >& _entitiesData, ecsSingletonComponents& _singletonComponents
// 												  , ComponentData< ecsPosition >& _positions
// 												  , ComponentData< ecsParticle >&						/*_particles*/
// 												  , ComponentData< ecsSunlightParticleOcclusion >&	/*_occlusion*/ )
// 	{
// 		// Get singleton components 
// 		ecsSunLightMesh_s& sunLight = _singletonComponents.GetComponent<ecsSunLightMesh_s>();
// 
// 		for ( int entity = 0; entity < _count; entity++ )
// 		{
// 			ecsComponentsKey& key = _entitiesData[ entity ];
// 
// 			if ( key.IsAlive() && key.MatchSignature( signature::bitset ) )
// 			{
// 				// get data
// 				btVector3& position = _positions.At( key ).position;
// 
// 				// raycast on the light mesh
// 				const btVector3 rayOrigin = btVector3( position[ 0 ], 1.f, position[ 2 ] );
// 				btVector3 outIntersection;
// 				bool isInsideSunlight = sunLight.mesh->RayCast( rayOrigin, -btVector3::Up(), outIntersection );
// 				if ( !isInsideSunlight )
// 				{
// 					key.Kill();
// 				}
// 			}
// 		}
// 	}