#include "project_empty/fanEmptyProject.hpp"

#include "core/time/fanProfiler.hpp"
#include "engine/systems/fanUpdateRenderWorld.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void EmptyProject::Init()
	{
        EcsIncludeBase(mWorld);
        EcsIncludePhysics(mWorld);
        EcsIncludeRender3D(mWorld);
        EcsIncludeRenderUI(mWorld);
        EcsIncludeNetworkClient( mWorld );

        mName = "empty_project";
	}

	//========================================================================================================
	//========================================================================================================
	void EmptyProject::Start()
	{

	}

	//========================================================================================================
	//========================================================================================================
	void  EmptyProject::Stop()
	{

	}

	//========================================================================================================
	//========================================================================================================
	void  EmptyProject::Step( const float /*_delta*/ )
	{
	}

    //========================================================================================================
    //========================================================================================================
    void EmptyProject::UpdateRenderWorld()
    {
        SCOPED_PROFILE( update_render_world );
        mWorld.ForceRun<SUpdateRenderWorldModels>();
        mWorld.ForceRun<SUpdateRenderWorldUI>();
        mWorld.ForceRun<SUpdateRenderWorldPointLights>();
        mWorld.ForceRun<SUpdateRenderWorldDirectionalLights>();
    }
}