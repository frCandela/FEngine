#pragma once

#include "engine/project/fanIProject.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	class GameServer : public IProject
	{
	public:
		void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;
        void UpdateRenderWorld() override;

	private:
        void UseGameCamera();
	};					  
}
