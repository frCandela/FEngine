#include "game/singletons/fanSelection.hpp"
#include "engine/fanEngineSerializable.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::SetInfo( EcsSingletonInfo& _info )
    {
        _info.save = &Selection::Save;
        _info.load = &Selection::Load;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Init( EcsWorld& /*_world*/, EcsSingleton& _component )
    {
        Selection& selection = static_cast<Selection&>( _component );
        selection.mSelectionFramePrefab = nullptr;
        selection.mSelectionFrames.clear();
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Save( const EcsSingleton& _component, Json& _json )
    {
        const Selection& selection = static_cast<const Selection&>( _component );
        Serializable::SaveResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab.mData );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void Selection::Load( EcsSingleton& _component, const Json& _json )
    {
        Selection& selection = static_cast<Selection&>( _component );
        Serializable::LoadResourcePtr( _json, "selection_frame", selection.mSelectionFramePrefab.mData );
    }
}