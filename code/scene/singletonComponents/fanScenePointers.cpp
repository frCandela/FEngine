#include "scene/singletonComponents/fanScenePointers.hpp"

#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::SCENE16;
		_info.init = &ScenePointers::Init;
		_info.onGui = &ScenePointers::OnGui;
		_info.name = "scene pointers";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
		scenePointers.unresolvedComponentPtr.clear();;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::OnGui( EcsWorld&, EcsSingleton& _component )
	{
		ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::Text( "unresolved component pointers: %d", scenePointers.unresolvedComponentPtr.size() );
		}
		ImGui::Unindent(); ImGui::Unindent();
	}

	//================================================================================================================================
	// Initializes all unresolved ComponentPointers with the corresponding  sceneNode & component of the scene
	// When instancing from prefab, scene nodes unique ids are offset by the greatest id in the scene
	// it is then necessary to offset the id of pointers as well using the field _idOffset
	//================================================================================================================================
	void  ScenePointers::ResolveComponentPointers( Scene& _scene, const uint32_t _idOffset )
	{
		EcsWorld& world = *_scene.world;
		ScenePointers& scenePointers = world.GetSingleton<ScenePointers>();

		while( !scenePointers.unresolvedComponentPtr.empty() )
		{
			ComponentPtrBase* ptr = *scenePointers.unresolvedComponentPtr.begin();
			ptr->sceneNodeID += _idOffset;
			assert( _scene.nodes.find( ptr->sceneNodeID ) != _scene.nodes.end() );
			const EcsHandle nodeHandle = _scene.nodes[ptr->sceneNodeID];
			SceneNode& node = world.GetComponent<SceneNode>( world.GetEntity(nodeHandle));
			EcsEntity entity = world.GetEntity( node.handle );
			assert( world.HasComponent( entity, ptr->staticID ) );
			EcsComponent& component = world.GetComponent( entity, ptr->staticID );
			ptr->Create( node, component );
		}
	}
}