#include "game/systems/fanUpdateGameUI.hpp"

#include "ecs/fanEcsWorld.hpp"
#include "game/components/fanSpaceShipUI.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"
#include "scene/components/fanCamera.hpp"
#include "scene/components/fanTransform.hpp"
#include "scene/components/ui/fanTransformUI.hpp"
#include "scene/singletonComponents/fanScene.hpp"
#include "scene/singletonComponents/fanRenderWorld.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateGameUiValues::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<SpaceshipUI>() |
			_world.GetSignature<SolarPanel>() |
			_world.GetSignature<Health>() |
			_world.GetSignature<Battery>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateGameUiValues::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		for( EntityID entityID : _entities )
		{	
			SpaceshipUI& ui = _world.GetComponent<SpaceshipUI>( entityID );

			if( ui.energyProgress == nullptr || 
				ui.signalProgress == nullptr || 
				ui.signalRenderer == nullptr || 
				ui.healthProgress == nullptr )
			{
				continue;
			}

			// Update energy progress bar
			Battery& battery = _world.GetComponent<Battery>( entityID );
			ui.energyProgress->SetProgress( battery.currentEnergy / battery.maxEnergy );

			// Update signal progress bar
			SolarPanel& solarPanel = _world.GetComponent<SolarPanel>( entityID );
			const float signalRatio = solarPanel.currentChargingRate / solarPanel.maxChargingRate;
			ui.signalProgress->SetProgress( signalRatio );

			if( signalRatio < 0.5f )
			{
				ui.signalRenderer->color = Color( 1.f, 2.f * signalRatio, 0.f, 1.f ); // red to orange
			}
			else
			{
				float ratio = 2.f * ( signalRatio - 0.5f );
				ui.signalRenderer->color = Color( 1.f - signalRatio, 1.f, 0.f, 1.f ); // orange to green
			}

			// Update health progress bar
			Health& health = _world.GetComponent<Health>( entityID );
			ui.healthProgress->SetProgress( health.currentHealth / health.maxHealth );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	Signature S_UpdateGameUiPosition::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<SpaceshipUI>() |
			_world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateGameUiPosition::Run( EcsWorld& _world, const std::vector<EntityID>& _entities, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		Scene& scene = _world.GetSingletonComponent<Scene>();
		EntityID cameraID = _world.GetEntityID( scene.mainCamera->handle );
		Camera& camera = _world.GetComponent<Camera>( cameraID );
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
		RenderWorld& renderWorld = _world.GetSingletonComponent<RenderWorld>();

		for( EntityID entityID : _entities )
		{
			SpaceshipUI& ui = _world.GetComponent<SpaceshipUI>( entityID );
			if( ui.uiRootTransform == nullptr ) { continue; }

			// Set ui position
			Transform& transform = _world.GetComponent<Transform>( entityID );
			glm::vec2 screenPos = ToGLM( camera.WorldPosToScreen( cameraTransform, transform.GetPosition() ) );
			glm::vec2 pixelPosition = renderWorld.targetSize * 0.5f * ( screenPos + glm::vec2( 1.f, 1.f ) );
			ui.uiRootTransform->position = pixelPosition + ui.uiOffset;
		}
	}
}