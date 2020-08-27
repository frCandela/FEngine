#include "scene/components/fanBounds.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Bounds::SetInfo( EcsComponentInfo& _info )
	{
		_info.mGroup = EngineGroups::Scene;
		_info.mName  = "bounds";
	}

	//========================================================================================================
	//========================================================================================================
	void Bounds::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		Bounds& bounds = static_cast<Bounds&>( _component );
		bounds.mAabb = AABB();
	}
}