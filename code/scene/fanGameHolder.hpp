#pragma once

#include "core/memory/fanSerializedValues.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "scene/fanIHolder.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"

namespace fan
{
	class Window;
	struct Scene;

	//========================================================================================================
	// This holder displays the game
	// Auto load a scene & displays it, no editor features
	// runs the main loop & manages events
	//========================================================================================================
	class GameHolder : public IHolder
	{
	public:
		GameHolder( LaunchSettings& _settings, IGame& _game );
		~GameHolder() override {};

		void Run();
		void Step();

	private:
        IGame& mGame;

        LaunchSettings& AdaptSettings( LaunchSettings& _settings );
	};
}
