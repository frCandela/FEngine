#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct Ray;

	//==============================================================================================================================================================
	// raycast on entity bounds then on a the convex shape of the mesh when present
	// return true if the raycast succeeded and output a list of EntityID sorted by distance to the ray origin
	//==============================================================================================================================================================
	struct S_RaycastAll : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static bool Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const Ray& _ray, std::vector<EntityID>& _outResults );
	};
}