#pragma once

#include "fanDisableWarnings.hpp"
WARNINGS_GLM_PUSH()
#include "glm/glm.hpp"
WARNINGS_POP()
#include "core/fanSerializedValues.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "render/fanWindow.hpp"
#include "scene/fanPrefabManager.hpp"
#include "game/fanGameClient.hpp"
#include "game/fanGameServer.hpp"
#include "game/fanLaunchSettings.hpp"

namespace fan
{
	class IGame;
	class Renderer;
	class Window;
	struct Scene;

	//================================================================================================================================
	// This holder displays the game
	// Auto load a scene & dispays it, no editor features
	// Contains an ecsWorld, window & renderer
	// runs the main loop & manages events
	//================================================================================================================================	
	class GameHolder
	{
	public:
		Signal <> onLPPSynch;

		GameHolder( const LaunchSettings _settings, IGame& _game );
		~GameHolder();

		void Run();
		void Exit();
		void Step();

	private:
		Renderer*            mRenderer;
		Window               mWindow;
		PrefabManager        mPrefabManager;
        IGame&               mGame;
		double               mLastRenderTime = 0.;
		const LaunchSettings mLaunchSettings;
		bool                 mApplicationShouldExit;

		void UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size );
	};
}
