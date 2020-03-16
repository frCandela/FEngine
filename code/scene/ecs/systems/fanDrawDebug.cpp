#include "scene/ecs/systems/fanDrawDebug.hpp"

#include "scene/ecs/components/fanBounds.hpp"
#include "scene/ecs/components/fanSceneNode.hpp"
#include "render/fanRendererDebug.hpp"
#include "scene/ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_DrawDebugBounds::GetSignature( const EcsWorld& _world )
	{
		return	_world.GetSignature<Bounds>() | _world.GetSignature<SceneNode>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_DrawDebugBounds::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		for( EntityID entityID : _entities )
		{
			const Bounds& bounds = _world.GetComponent<Bounds>( entityID );
			const SceneNode& node = _world.GetComponent<SceneNode>( entityID );
			RendererDebug::Get().DebugAABB( bounds.aabb, Color::Red );
		}
	}
}