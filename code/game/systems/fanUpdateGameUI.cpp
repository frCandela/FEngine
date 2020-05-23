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
	EcsSignature S_UpdateGameUiValues::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<SpaceshipUI>() |
			_world.GetSignature<SolarPanel>() |
			_world.GetSignature<Health>() |
			_world.GetSignature<Battery>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateGameUiValues::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		auto uiIt = _view.begin<SpaceshipUI>();
		auto batteryIt = _view.begin<Battery>();
		auto solarPanelIt = _view.begin<SolarPanel>();
		auto healthIt = _view.begin<Health>();
		for( ; uiIt != _view.end<SpaceshipUI>(); ++uiIt, ++batteryIt, ++solarPanelIt, ++healthIt )
		{
			const SpaceshipUI& ui = *uiIt;
			const Battery& battery = *batteryIt;
			const SolarPanel& solarPanel = *solarPanelIt;
			const Health& health = *healthIt;

			if( ui.energyProgress == nullptr || 
				ui.signalProgress == nullptr || 
				ui.signalRenderer == nullptr || 
				ui.healthProgress == nullptr )
			{
				continue;
			}

			// Update energy progress bar
			ui.energyProgress->SetProgress( battery.currentEnergy / battery.maxEnergy );

			// Update signal progress bar
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
			ui.healthProgress->SetProgress( health.currentHealth / health.maxHealth );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	EcsSignature S_UpdateGameUiPosition::GetSignature( const EcsWorld& _world )
	{
		return
			_world.GetSignature<SpaceshipUI>() |
			_world.GetSignature<Transform>();
	}

	//================================================================================================================================
	//================================================================================================================================
	void S_UpdateGameUiPosition::Run( EcsWorld& _world, const EcsView& _view, const float _delta )
	{
		if( _delta == 0.f ) { return; }

		Scene& scene = _world.GetSingleton<Scene>();
		EcsEntity cameraID = _world.GetEntity( scene.mainCameraHandle );
		Camera& camera = _world.GetComponent<Camera>( cameraID );
		Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
		RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

		auto uiIt = _view.begin<SpaceshipUI>();
		auto transformIt = _view.begin<Transform>();
		for( ; uiIt != _view.end<SpaceshipUI>(); ++uiIt, ++transformIt )
		{
			const SpaceshipUI& ui = *uiIt;
			const Transform& transform = *transformIt;

			if( ui.uiRootTransform == nullptr ) { continue; }

			// Set ui position
			glm::vec2 screenPos = ToGLM( camera.WorldPosToScreen( cameraTransform, transform.GetPosition() ) );
			glm::vec2 pixelPosition = renderWorld.targetSize * 0.5f * ( screenPos + glm::vec2( 1.f, 1.f ) );
			ui.uiRootTransform->position = pixelPosition + ui.uiOffset;
		}
	}
}