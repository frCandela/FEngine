#pragma once

#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
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
		GameHolder( const LaunchSettings& _settings, IGame& _game );
		~GameHolder();

		void Run();
		void Step();

	private:
        IGame&               mGame;

        void GetInitialWindowPositionAndSize( glm::ivec2& _position, glm::ivec2& _size ) const;
	};
}
