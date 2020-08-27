#include "ecs/fanSlot.hpp"
#include "scene/singletons/fanScenePointers.hpp"
#include "scene/components/fanSceneNode.hpp"
#include "scene/singletons/fanScene.hpp"
#include "scene/fanSceneResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ScenePointers::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::PTR16;
		_info.group = EngineGroups::Scene;
		_info.onGui = &ScenePointers::OnGui;
		_info.name = "scene pointers";
	}

	//========================================================================================================
	//========================================================================================================
	void ScenePointers::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
	{
		ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
		scenePointers.mUnresolvedComponentPtr.clear();
	}

    //========================================================================================================
    //========================================================================================================
    void ScenePointers::Clear( ScenePointers& _scenePointers )
	{
        _scenePointers.mUnresolvedComponentPtr.clear();
        _scenePointers.mUnresolvedComponentPtr.clear();
	}

	//========================================================================================================
	// Initializes all unresolved ComponentPointers with the corresponding  sceneNode & component of the scene
	// When instancing from prefab, scene nodes unique ids are offset by the greatest id in the scene
	// it is then necessary to offset the id of pointers as well using the field _idOffset
	//========================================================================================================
	void  ScenePointers::ResolveComponentPointers( EcsWorld& _world, const uint32_t _idOffset )
	{
		ScenePointers& scenePointers = _world.GetSingleton<ScenePointers>();
		for ( ComponentPtrBase* componentPtr : scenePointers.mUnresolvedComponentPtr )
		{
			componentPtr->handle += _idOffset;
		}
		scenePointers.mUnresolvedComponentPtr.clear();

        for( SlotPtr* slotPtr : scenePointers.mUnresolvedSlotPtr )
        {
            const SlotPtr::SlotCallData& data = slotPtr->Data();
            slotPtr->SetComponentSlot( data.mHandle + _idOffset, data.mType, data.mSlot );
        }
        scenePointers.mUnresolvedSlotPtr.clear();
	}

    //========================================================================================================
    //========================================================================================================
    void ScenePointers::OnGui( EcsWorld&, EcsSingleton& _component )
    {
        ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
        ImGui::Text( "unresolved component pointers: %d", scenePointers.mUnresolvedComponentPtr.size() );
    }
}