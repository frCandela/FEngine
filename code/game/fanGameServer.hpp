#pragma once

#include "scene/fanIGame.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	class GameServer : public IGame
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
