#pragma once

#include "scene/fanGameBase.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class GameServer : public GameBase
	{
	public:
		void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;
	};					  
}
