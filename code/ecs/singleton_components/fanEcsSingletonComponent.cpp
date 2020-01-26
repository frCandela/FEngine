#include "fanGlobalIncludes.h"
#include "ecs/singleton_components/fanEcsSingletonComponent.h"

#include "render/fanMesh.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	ecsParticlesMesh_s::ecsParticlesMesh_s()
	{
		mesh = new Mesh();

		mesh->LoadFromFile( "particles mesh" );
		mesh->SetHostVisible( true );
		mesh->SetOptimizeVertices( false );
		mesh->SetAutoUpdateHull( false );
	}

	//================================================================================================================================
	//================================================================================================================================
	ecsParticlesMesh_s::~ecsParticlesMesh_s()
	{
		delete mesh;
	}
}