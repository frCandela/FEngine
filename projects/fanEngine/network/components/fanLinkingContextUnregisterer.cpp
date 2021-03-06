#include "network/components/fanLinkingContextUnregisterer.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "network/singletons/fanLinkingContext.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void LinkingContextUnregisterer::SetInfo( EcsComponentInfo& _info )
    {
        _info.destroy = &LinkingContextUnregisterer::Destroy;
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void LinkingContextUnregisterer::Init( EcsWorld&, EcsEntity /*_entity*/, EcsComponent& /*_cpnt*/ )
    {
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    void LinkingContextUnregisterer::Destroy( EcsWorld& _world, EcsEntity _entity, EcsComponent& /*_cpnt*/ )
    {
        LinkingContext& linkingContext = _world.GetSingleton<LinkingContext>();
        linkingContext.RemoveEntity( _world.GetHandle( _entity ) );
    }
}