#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanSystem.hpp"

namespace fan
{
	class EcsWorld;
	struct PlayerInput;

	//==============================================================================================================================================================
	// refresh all PlayerInput components
	//==============================================================================================================================================================
	struct S_RefreshPlayerInput : System
	{
		static Signature GetSignature( const EcsWorld& _world );
		static void Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta );

	private:
		static glm::vec2 GetDirectionAverage( PlayerInput& _input );
		static float	 GetInputLeft( const PlayerInput& _input );
		static float	 GetInputForward( const PlayerInput& _input );
		static float	 GetInputBoost( const PlayerInput& _input );
		static float	 GetInputFire( const PlayerInput& _input );
		static bool		 GetInputStop( const PlayerInput& _input );
	};
}