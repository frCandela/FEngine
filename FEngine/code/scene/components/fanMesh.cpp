#include "fanIncludes.h"

#include "scene/components/fanMesh.h"
#include "fanEngine.h"
#include "vulkan/vkRenderer.h"



namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	Mesh::~Mesh() {
		fan::Engine::GetEngine().GetRenderer().RemoveMesh(this);		
	}
}