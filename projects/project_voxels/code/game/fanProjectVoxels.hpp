#pragma once

#include "fanGlm.hpp"
#include "engine/project/fanIProject.hpp"

namespace fan
{
	//==========================================================================================================================
	//==========================================================================================================================
	class ProjectVoxels : public IProject
	{
	public:
        ProjectVoxels();
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const Fixed _delta ) override;
        void Render() override;
        void OnGui() override;

        void StepLoadTerrain();
	};
}
