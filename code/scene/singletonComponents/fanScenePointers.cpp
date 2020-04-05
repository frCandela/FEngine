#include "scene/singletonComponents/fanScenePointers.hpp"

#include "scene/components/fanSceneNode.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
	REGISTER_SINGLETON_COMPONENT( ScenePointers	 );

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::SetInfo( SingletonComponentInfo& _info )
	{
		_info.icon = ImGui::SCENE16;
		_info.init = &ScenePointers::Init;
		_info.onGui = &ScenePointers::OnGui;
		_info.name = "scene pointers";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::Init( EcsWorld& _world, SingletonComponent& _component )
	{
		ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
		scenePointers.unresolvedComponentPtr.clear();;
	}

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::OnGui( SingletonComponent& _component )
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
		ScenePointers& scenePointers = world.GetSingletonComponent<ScenePointers>();

		while( !scenePointers.unresolvedComponentPtr.empty() )
		{
			ComponentPtrBase* ptr = *scenePointers.unresolvedComponentPtr.begin();
			ptr->sceneNodeID += _idOffset;
			assert( _scene.nodes.find( ptr->sceneNodeID ) != _scene.nodes.end() );
			SceneNode& node = *_scene.nodes[ptr->sceneNodeID];
			EntityID entityID = world.GetEntityID( node.handle );
			assert( world.HasComponent( entityID, ptr->dynamicID ) );
			Component& component = world.GetComponent( entityID, ptr->dynamicID );
			ptr->Create( node, component );
		}
	}
}