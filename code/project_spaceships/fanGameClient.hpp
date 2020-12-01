#pragma once

#include "engine/project/fanIProject.hpp"

namespace fan
{
    struct Scene;

	//========================================================================================================
	//========================================================================================================
	class GameClient : public IProject
	{
	public:
        void Init() override;
        void Start() override;
        void Stop() override;
        void Step( const float _delta ) override;
        void UpdateRenderWorld() override;

		static void CreateGameAxes();

	private:
		void RollbackResimulate( const float _delta );
        void UseGameCamera();
        void OnLoadScene( Scene& _scene );
	};
}
