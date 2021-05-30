#include "core/ecs/fanSlot.hpp"
#include "engine/singletons/fanScenePointers.hpp"
#include "engine/singletons/fanScene.hpp"
#include "engine/resources/fanComponentPtr.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ScenePointers::SetInfo( EcsSingletonInfo& /*_info*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ScenePointers::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        ScenePointers& scenePointers = static_cast<ScenePointers&>( _component );
        scenePointers.mUnresolvedComponentPtr.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ScenePointers::Clear( ScenePointers& _scenePointers )
    {
        _scenePointers.mUnresolvedComponentPtr.clear();
        _scenePointers.mUnresolvedComponentPtr.clear();
    }

    //==================================================================================================================================================================================================
    // Initializes all unresolved ComponentPointers with the corresponding  sceneNode & component of the scene
    // When instancing from prefab, scene nodes unique ids are offset by the greatest id in the scene
    // it is then necessary to offset the id of pointers as well using the field _idOffset
    //==================================================================================================================================================================================================
    void ScenePointers::ResolveComponentPointers( EcsWorld& _world, const uint32_t _idOffset )
    {
        ScenePointers        & scenePointers = _world.GetSingleton<ScenePointers>();
        for( ComponentPtrBase* componentPtr : scenePointers.mUnresolvedComponentPtr )
        {
            componentPtr->mHandle += _idOffset;
        }
        scenePointers.mUnresolvedComponentPtr.clear();

        for( SlotPtr* slotPtr : scenePointers.mUnresolvedSlotPtr )
        {
            const SlotPtr::SlotCallData& data = slotPtr->Data();
            slotPtr->SetComponentSlot( data.mHandle + _idOffset, data.mType, data.mSlot );
        }
        scenePointers.mUnresolvedSlotPtr.clear();
    }
}