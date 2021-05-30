#include "fanComponentPtr.hpp"

#include "engine/singletons/fanScenePointers.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    // initializes the ComponentPtr from ids :
    // - _sceneNodeID is the unique index of the target scene node on which the component is attached
    // - _staticID is the static id of the component
    //==================================================================================================================================================================================================
    void ComponentPtrBase::CreateUnresolved( EcsHandle _handle )
    {
        mHandle = _handle;

        // adds to the unresolved pointers list
        if( _handle != 0 )
        {
            ScenePointers& scenePointers = mWorld->GetSingleton<ScenePointers>();
            scenePointers.mUnresolvedComponentPtr.push_back( this );
        }
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ComponentPtrBase::Create( EcsHandle _handle )
    {
        mHandle = _handle;
        fanAssert( mWorld->HasComponent( mWorld->GetEntity( _handle ), mType ) );
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void ComponentPtrBase::Clear()
    {
        mHandle = 0;
    }
}