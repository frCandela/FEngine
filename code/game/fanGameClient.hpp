#pragma once

#include "scene/fanIGame.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class GameClient : public IGame
	{
	public:
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;

		void RollbackResimulate( const float _delta );
		static void CreateGameAxes();
	};
}
