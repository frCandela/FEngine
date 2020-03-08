#pragma once

#include "scene/fanScenePrecompiled.hpp"

namespace fan
{
	class Mesh;

	class EcsSingletonComponent {};

	//================================================================================================================================
	// Position of the current camera in the scene
	//================================================================================================================================
	class ecsCameraPosition_s
	{
	public:
		btVector3 position;
	};

	//================================================================================================================================
	// Mesh used to represent the particles in the scene
	//================================================================================================================================
	class ecsParticlesMesh_s
	{
	public:
		ecsParticlesMesh_s();
		~ecsParticlesMesh_s();

		Mesh* mesh;
	};
}