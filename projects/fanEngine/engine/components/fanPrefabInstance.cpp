#include "engine/components/fanPrefabInstance.hpp"
#include "core/memory/fanSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::SetInfo( EcsComponentInfo&  )
    {
        // we don't want to save/load because prefabs are a special case of serialization
        // _info.load = &PrefabInstance::Load;
        // _info.save = &PrefabInstance::Save;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
    {
        PrefabInstance& prefabInstance = static_cast<PrefabInstance&>( _component );
        prefabInstance.mPrefab = nullptr;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::Save( const EcsComponent& _component, Json& _json )
    {
        const PrefabInstance& prefabInstance = static_cast<const PrefabInstance&>( _component );
        Serializable::SaveResourcePtr( _json, "prefab", prefabInstance.mPrefab );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::Load( EcsComponent& _component, const Json& _json )
    {
        PrefabInstance& prefabInstance = static_cast<PrefabInstance&>( _component );
        Serializable::LoadResourcePtr( _json, "prefab", prefabInstance.mPrefab );
    }
}