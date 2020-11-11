#prama once

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SolarEruption::SetInfo( EcsSingletonInfo& _info )
	{
		_info.mIcon  = ImGui::SunEruption16;
		_info.mGroup = EngineGroups::Game;
		_info.onGui  = &SolarEruption::OnGui;
		_info.mName  = "solar eruption";
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::OnGui( EcsWorld& _world, EcsSingleton& _component )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );
		const Time& time = _world.GetSingleton<Time>();
		ImGui::PushItemWidth( 200.f );
		{
			if( ImGui::Button("spawn") ){ solarEruption.SpawnEruptionNow(); }
			ImGui::Text( "state: %s", StateToString( solarEruption.mState ).c_str() );
			ImGui::Text( "timer: %f", solarEruption.mTimer );
            const float nextEruptionTime = time.mLogicDelta *
                                           float ( solarEruption.mSpawnFrame - time.mFrameIndex );
			ImGui::Text( "next eruption: %f", nextEruptionTime );

			ImGui::Spacing();

			// misc
			if( ImGui::CollapsingHeader( "global settings" ) )
			{
				ImGui::Checkbox( "enabled", &solarEruption.mEnabled );
				ImGui::DragFloat( "cooldown", &solarEruption.mCooldown );
				ImGui::DragFloat( "random cooldown", &solarEruption.mRandomCooldown );
				ImGui::DragFloat( "damage per second", &solarEruption.mDamagePerSecond, 1.f, 0.f, 100.f );
				if( ImGui::Button( "Eruption" ) ) { solarEruption.mStateDuration[WAITING] = 0.f; }
			}

			// states parameters
			for( int i = 0; i < SIZE; i++ )
			{
				std::string name = StateToString( State( i ) );
				if( ImGui::CollapsingHeader( ( "state: " + name ).c_str() ) )
				{
                    ImGui::DragFloat( ( "duration##" + name ).c_str(), &solarEruption.mStateDuration[i] );
                    ImGui::DragFloat( ( "particle per second##" + name ).c_str(),
                                      &solarEruption.mStateParticlePerSecond[i] );
                    ImGui::DragFloat( ( "particle speed##" + name ).c_str(),
                                      &solarEruption.mStateParticleSpeed[i] );
                    ImGui::DragFloat( ( "particle duration##" + name ).c_str(),
                                      &solarEruption.mStateParticleDuration[i] );
                    ImGui::DragFloat( ( "linear light attenuation##" + name ).c_str(),
                                      &solarEruption.mStateLightAttenuation[i],
                                      0.001f,
                                      0.f,
                                      1.f );
                    ImGui::ColorEdit4( ( "state particle color##" + name ).c_str(),
                                       solarEruption.mStateParticleColor[i].Data(),
                                       ImGui::fanColorEditFlags );
				}
			}

			ImGui::Spacing();
		} ImGui::PopItemWidth();

		// components
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			ImGui::FanComponent<Material>( "sunlight material", solarEruption.mSunlightMaterial );
			ImGui::FanComponent<MeshRenderer>( "sunlight renderer", solarEruption.mSunlightRenderer );
			ImGui::FanComponent<ParticleEmitter>( "particle emitter", solarEruption.mParticleEmitter );
			ImGui::FanComponent<PointLight>( "sunlight light", solarEruption.mSunlightLight );
		} ImGui::PopItemWidth();
	}
}