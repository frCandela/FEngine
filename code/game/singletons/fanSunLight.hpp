#pragma once

#include "core/ecs/fanEcsSingleton.hpp"
#include "render/resources/fanMesh.hpp"

namespace fan
{
	//========================================================================================================
	// SunLight is a mesh generated from the position/scale of the planets to simulate light 
	// -subAngle is the minimal angle between two radial segments of the mesh
	//========================================================================================================
	class SunLight : public EcsSingleton
	{
		ECS_SINGLETON( SunLight )
		static void SetInfo( EcsSingletonInfo& _info );
		static void Init( EcsWorld& _world, EcsSingleton& _component );
		static void Save( const EcsSingleton& _component, Json& _json );
		static void Load( EcsSingleton& _component, const Json& _json );

		Mesh * mMesh = nullptr;
		float  mSubAngle;
		float  mRadius;

		void AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 );
	};
}
