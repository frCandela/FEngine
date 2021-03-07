#pragma once

#include "engine/project/fanIProject.hpp"

namespace fan
{
	//==========================================================================================================================
	//==========================================================================================================================
	class ProjectEmpty : public IProject
	{
	public:
        ProjectEmpty();
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;
        void UpdateRenderWorld() override;
        void OnGui() override;
	};
}
