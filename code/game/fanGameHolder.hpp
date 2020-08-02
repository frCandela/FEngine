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
	class EcsWorld;
	class Renderer;
	class Window;
	struct Scene;

	//================================================================================================================================
	// This holder displays the game as it will be in retail.
	// Auto load a scene & dispays it, no editor features
	// Contains an ecsWorld, window & renderer
	// runs the main loop & manages events
	//================================================================================================================================	
	class GameHolder
	{
	public:
		Signal <> onLPPSynch;

		GameHolder( const LaunchSettings _settings, EcsWorld&  _world );
		~GameHolder();

		void Run();
		void Exit();
		void Step();
			   
	private:		
		Renderer* m_renderer;
		Window m_window;
		PrefabManager mPrefabManager;
		EcsWorld& m_world;
		double m_lastRenderTime = 0.;
		const LaunchSettings m_launchSettings;

		bool m_applicationShouldExit;

		static void GameStart( EcsWorld& _world );
		static void GameStop( EcsWorld& _world );
		static void GameStep( EcsWorld& _world, const float _delta );		
		static void UpdateRenderWorld( Renderer& _renderer, EcsWorld& _world, const glm::vec2 _size );
	};
}
