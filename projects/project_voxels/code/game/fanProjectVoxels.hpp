#pragma once

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
        void Step( const float _delta ) override;
        void UpdateRenderWorld() override;
        void OnGui() override;
	};
}
