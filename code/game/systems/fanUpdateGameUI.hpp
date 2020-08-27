#include "ecs/fanEcsSystem.hpp"
#include "scene/components/fanTransform.hpp"
#include "game/components/fanSpaceShipUI.hpp"
#include "game/components/fanBattery.hpp"
#include "game/components/fanSolarPanel.hpp"
#include "game/components/fanHealth.hpp"

namespace fan
{
	//========================================================================================================
	// updates the ui sliders from the player/world variables ( health, energy, etc. )
	//========================================================================================================
	struct SUpdateGameUiValues : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<SpaceshipUI>() |
				_world.GetSignature<SolarPanel>() |
				_world.GetSignature<Health>() |
				_world.GetSignature<Battery>();
		}

		static void Run( EcsWorld& /*_world*/, const EcsView& _view, const float _delta )
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

				if( ui.mEnergyProgress == nullptr ||
                    ui.mSignalProgress == nullptr ||
                    ui.mSignalRenderer == nullptr ||
                    ui.mHealthProgress == nullptr )
				{
					continue;
				}

				// Update energy progress bar
				ui.mEnergyProgress->SetProgress( battery.mCurrentEnergy / battery.mMaxEnergy );

				// Update signal progress bar
				const float signalRatio = solarPanel.mCurrentChargingRate / solarPanel.mMaxChargingRate;
				ui.mSignalProgress->SetProgress( signalRatio );

				if( signalRatio < 0.5f )
				{
					ui.mSignalRenderer->mColor = Color( 1.f, 2.f * signalRatio, 0.f, 1.f ); // red to orange
				}
				else
				{
					ui.mSignalRenderer->mColor = Color( 1.f - signalRatio, 1.f, 0.f, 1.f ); // orange to green
				}

				// Update health progress bar
				ui.mHealthProgress->SetProgress( health.mCurrentHealth / health.mMaxHealth );
			}
		}
	};

	//========================================================================================================
	// moves the ui status bars above the player spaceship 
	//========================================================================================================
	struct SUpdateGameUiPosition : EcsSystem
	{
		static EcsSignature GetSignature( const EcsWorld& _world )
		{
			return
				_world.GetSignature<SpaceshipUI>() |
				_world.GetSignature<Transform>();
		}

		static void Run( EcsWorld& _world, const EcsView& _view, const float _delta )
		{
			if( _delta == 0.f ) { return; }

			Scene& scene = _world.GetSingleton<Scene>();
			EcsEntity cameraID = _world.GetEntity( scene.mMainCameraHandle );
			Camera& camera = _world.GetComponent<Camera>( cameraID );
			Transform& cameraTransform = _world.GetComponent<Transform>( cameraID );
			RenderWorld& renderWorld = _world.GetSingleton<RenderWorld>();

			auto uiIt = _view.begin<SpaceshipUI>();
			auto transformIt = _view.begin<Transform>();
			for( ; uiIt != _view.end<SpaceshipUI>(); ++uiIt, ++transformIt )
			{
				const SpaceshipUI& ui = *uiIt;
				const Transform& transform = *transformIt;

				if( ui.mUIRootTransform == nullptr ) { continue; }

				// Set ui position
                glm::vec2 screenPos = ToGLM( camera.WorldPosToScreen( cameraTransform,
                                                                      transform.GetPosition() ) );
                glm::vec2 pixelPosition = renderWorld.mTargetSize *
                                          0.5f *
                                          ( screenPos + glm::vec2( 1.f, 1.f ) );
				ui.mUIRootTransform->mPosition = pixelPosition + ui.mUIOffset;
			}
		}
	};
}