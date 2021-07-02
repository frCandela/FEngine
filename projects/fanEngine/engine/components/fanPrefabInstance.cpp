#include "engine/components/fanPrefabInstance.hpp"
#include "core/memory/fanSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::SetInfo( EcsComponentInfo& )
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
        prefabInstance.mPrefab       = nullptr;
        prefabInstance.mOverrideType = HasNone;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::CreateOverride( PrefabInstance& _prefabInstance, EcsWorld& _world, EcsEntity _entity )
    {
        if( _world.HasComponent<Transform>( _entity ) )
        {
            _prefabInstance.mOverrideType        = PrefabInstance::HasTransform;
            _prefabInstance.mOverride.mTransform = _world.GetComponent<Transform>( _entity );
        }
        else if( _world.HasComponent<UITransform>( _entity ) )
        {
            _prefabInstance.mOverrideType          = PrefabInstance::HasUITransform;
            _prefabInstance.mOverride.mUiTransform = _world.GetComponent<UITransform>( _entity );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::ApplyOverride( const PrefabInstance& _prefabInstance, EcsWorld& _world, EcsEntity _entity )
    {
        if( _prefabInstance.mOverrideType == PrefabInstance::HasUITransform && _world.HasComponent<UITransform>( _entity ) )
        {
            _world.GetComponent<UITransform>( _entity ) = _prefabInstance.mOverride.mUiTransform;
        }
        if( _prefabInstance.mOverrideType == PrefabInstance::HasTransform && _world.HasComponent<Transform>( _entity ) )
        {
            _world.GetComponent<Transform>( _entity ) = _prefabInstance.mOverride.mTransform;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::Save( const EcsComponent& _component, Json& _json )
    {
        const PrefabInstance& prefabInstance = static_cast<const PrefabInstance&>( _component );
        Serializable::SaveResourcePtr( _json, "prefab", prefabInstance.mPrefab );
        Serializable::SaveInt( _json, "override_type", prefabInstance.mOverrideType );
        switch( prefabInstance.mOverrideType )
        {
            case HasNone:
                break;
            case HasTransform:
                Transform::Save( prefabInstance.mOverride.mTransform, _json["override"] );
                break;
            case HasUITransform:
                UITransform::Save( prefabInstance.mOverride.mUiTransform, _json["override"] );
                break;
            default:
                fanAssert( false );
                break;
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void PrefabInstance::Load( EcsComponent& _component, const Json& _json )
    {
        PrefabInstance& prefabInstance = static_cast<PrefabInstance&>( _component );
        Serializable::LoadResourcePtr( _json, "prefab", prefabInstance.mPrefab );
        Serializable::LoadInt( _json, "override_type", (int&)prefabInstance.mOverrideType );
        switch( prefabInstance.mOverrideType )
        {
            case HasNone:
                break;
            case HasTransform:
                Transform::Load( prefabInstance.mOverride.mTransform, _json["override"] );
                break;
            case HasUITransform:
                UITransform::Load( prefabInstance.mOverride.mUiTransform, _json["override"] );
                break;
            default:
                fanAssert( false );
                break;
        }
    }
}