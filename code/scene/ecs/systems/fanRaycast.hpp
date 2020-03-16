#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct Ray;

	//==============================================================================================================================================================
	// raycast on entity bounds
	// return true if the raycast succeeded 
	//==============================================================================================================================================================
	struct S_RaycastAll : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static bool Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const Ray& _ray, std::vector<EntityID>& _outResults );
	};
}