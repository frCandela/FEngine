#include "scene/ecs/singletonComponents/fanRenderWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( RenderWorld, "render_world" );

	RenderWorld::RenderWorld()
	{		
		particlesMesh.SetHostVisible( true );
		particlesMesh.SetOptimizeVertices( false );
		particlesMesh.SetAutoUpdateHull( false );
	}
}