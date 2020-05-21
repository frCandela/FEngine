#include "ecs/fanEcsSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct Ray;

	//==============================================================================================================================================================
	// raycast on entity bounds then on a the convex shape of the mesh when present
	// return true if the raycast succeeded and output a list of EcsEntity sorted by distance to the ray origin
	//==============================================================================================================================================================
	struct S_RaycastAll : EcsSystem
	{
		staticEcsSignatureGetSignature( const EcsWorld& _world );
		static bool Run( EcsWorld& _world, const std::vector<EcsEntity>& _entities, const Ray& _ray, std::vector<EcsEntity>& _outResults );
	};
}