#include "scene/ecs/fanSystem.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "render/fanRendererDebug.hpp"

namespace fan
{
	//==============================================================================================================================================================
	// Spawns particles for all particle emitters in the scene
	//==============================================================================================================================================================
	struct S_DrawDebugBounds : System
	{
	public:

		static Signature GetSignature( const EcsWorld& _world )
		{
			return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>();
		}

		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
		{
			for( EntityID entityID : _entities )
			{
				const Bounds& bounds  = _world.GetComponent<Bounds>( entityID );
				const SceneNode& node = _world.GetComponent<SceneNode>( entityID );
				RendererDebug::Get().DebugAABB( bounds.aabb, Color::Red );
			}
		}
	};
}