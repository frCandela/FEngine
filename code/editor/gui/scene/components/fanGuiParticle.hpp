#prama once

#include "scene/components/fanParticle.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void Particle::SetInfo( EcsComponentInfo& _info )
	{
		_info.mName  = "particle";
		_info.mGroup = EngineGroups::Scene;
	}
}