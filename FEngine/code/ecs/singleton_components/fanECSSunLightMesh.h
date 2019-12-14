#pragma once

#include "ecs/singleton_components/fanEcsSingletonComponent.h"

namespace fan
{
	class Mesh;
	struct Vertex;

	//================================================================================================================================
	//================================================================================================================================
	class ecsSunLightMesh_s : public EcsSingletonComponent
	{
	public:
		ecsSunLightMesh_s();
		~ecsSunLightMesh_s();

		Mesh* mesh;
		float subAngle;
		float radius;
		bool  debugDraw;

		void AddSunTriangle( std::vector<Vertex>& _vertices, const btVector3& _v0, const btVector3& _v1 );
	};
}