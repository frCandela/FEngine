#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSingletonComponent.hpp"
#include "render/fanMesh.hpp"

namespace fan
{
	//================================================================================================================================
	// SunLight is a mesh generated from the position/scale of the planets to simulate light 
	// -subAngle is the minimal angle between two radial segments of the mesh
	//================================================================================================================================
	class SunLight : public SingletonComponent
	{
		DECLARE_SINGLETON_COMPONENT()
	public:
		static void SetInfo( SingletonComponentInfo& _info );
		static void Init( EcsWorld& _world, SingletonComponent& _component );
		static void OnGui( SingletonComponent& _component );
		static void Save( const SingletonComponent& _component, Json& _json );
		static void Load( SingletonComponent& _component, const Json& _json );

		Mesh  mesh;
		float subAngle;
		float radius;
		bool  debugDraw;

		void AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 );
	};
}
