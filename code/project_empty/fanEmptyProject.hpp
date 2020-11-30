#pragma once

#include "engine/project/fanIGame.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	class EmptyProject : public IGame
	{
	public:
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;
        void UpdateRenderWorld() override;
	};
}
