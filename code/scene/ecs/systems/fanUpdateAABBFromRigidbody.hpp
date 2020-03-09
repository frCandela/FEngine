#include "scene/ecs/fanSystem.hpp"

namespace fan
{
	//==============================================================================================================================================================
	//==============================================================================================================================================================
	struct UpdateAABBFromRigidbodySystem : System
	{
	public:

		static Signature GetSignature( const EntityWorld& _world )
		{
			return _world.GetSignature<tag_editorOnly>();
		}

		static void Run( EntityWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{

		}
	};
}