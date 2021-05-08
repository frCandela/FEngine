#pragma once

#include "core/memory/fanSerializedValues.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "engine/project/fanIProjectContainer.hpp"
#include "network/fanNetConfig.hpp"

namespace fan
{
	class Window;
	struct Scene;

	//========================================================================================================
	// This container runs the game
	// Auto load a scene & displays it, no editor features
	// runs the main loop & manages events
	//========================================================================================================
	class GameProjectContainer : public IProjectContainer
	{
	public:
		GameProjectContainer( LaunchSettings& _settings, IProject& _project );
		~GameProjectContainer() override {};

		void Run() override;
		void Step();

	private:
        IProject&   mProject;
        FrameIndex  mLastLogicFrameRendered;

        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
	};
}
