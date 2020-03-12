#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct S_UpdateAABBFromRigidbodySystem : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return _world.GetSignature<tag_editorOnly>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{

		}
	};
}