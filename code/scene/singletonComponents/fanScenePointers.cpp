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
		_info.onGui = &ScenePointers::OnGui;
		_info.name = "scene pointers";
	}

	//================================================================================================================================
	//================================================================================================================================
	void ScenePointers::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
		scenePointers.unresolvedComponentPtr.clear();
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
	void  ScenePointers::ResolveComponentPointers( EcsWorld& _world, const uint32_t _idOffset )
	{
		ScenePointers& scenePointers = _world.GetSingleton<ScenePointers>();
		for ( ComponentPtrBase* componentPtr : scenePointers.unresolvedComponentPtr )
		{
			componentPtr->handle += _idOffset;
		}
		scenePointers.unresolvedComponentPtr.clear();
	}
}