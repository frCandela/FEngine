#include "game/singletonComponents/fanSolarEruption.hpp"

#include "core/fanDebug.hpp"
#include "editor/fanModals.hpp"
#include "scene/fanSceneSerializable.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "game/singletonComponents/fanSunLight.hpp"
#include "game/singletonComponents/fanGame.hpp"
#include "game/fanGameTags.hpp"
#include "core/fanRandom.hpp"
#include "core/math/fanLerp.hpp"
#include "network/fanPacket.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::SetInfo( EcsSingletonInfo& _info )
	{
		_info.icon = ImGui::SUN16;
		_info.init = &SolarEruption::Init;
		_info.onGui = &SolarEruption::OnGui;
		_info.save = &SolarEruption::Save;
		_info.load = &SolarEruption::Load;
		_info.netSave = &SolarEruption::NetSave;
		_info.netLoad = &SolarEruption::NetLoad;
		_info.name = "solar eruption";
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );

		solarEruption.sunlightMaterial.Init( _world );
		solarEruption.sunlightRenderer.Init( _world );
		solarEruption.particleEmitter.Init( _world );
		solarEruption.sunlightLight.Init( _world );

		solarEruption.timer = 0;
		solarEruption.enabled = false;
		solarEruption.state = State::WAITING;
		solarEruption.cooldown = 5.f;
		solarEruption.randomCooldown = 1.f;
		solarEruption.damagePerSecond = 10.f;

		for( int i = 0; i < SIZE; i++ )
		{
			solarEruption.stateDuration[i] = 2.f;
			solarEruption.stateParticlePerSecond[i] = 100.f;
			solarEruption.stateParticleSpeed[i] = 3.f;
			solarEruption.stateParticleDuration[i] = 3.f;
			solarEruption.stateParticleColor[i] = Color::Magenta;
		}
	}

	//================================================================================================================================
	// returns the next eruption start frame
	//================================================================================================================================
	FrameIndex SolarEruption::CalculateNextEruptionStartFrame( const SolarEruption& _eruption, const Game& game )
	{
		const float delay = _eruption.cooldown + Random::Float() * _eruption.randomCooldown;
		return game.frameIndex + (FrameIndex)( delay / game.logicDelta);
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Start( EcsWorld& _world )
	{
		SolarEruption& eruption = _world.GetSingleton<SolarEruption>();		
		const Game& game = _world.GetSingleton<Game>();

		eruption.enabled = true;
		eruption.stateDuration[WAITING] = std::numeric_limits<float>::max(); // wait forever
		eruption.eruptionStartFrame = std::numeric_limits<FrameIndex>::max(); // no eruption planned

		// pointers check
		if( eruption.sunlightMaterial == nullptr
			|| eruption.sunlightRenderer == nullptr
			|| eruption.particleEmitter == nullptr
			|| eruption.sunlightLight == nullptr )
		{
			Debug::Error() << "SolarEruption : missing component references." << Debug::Endl;
			eruption.enabled = false;
		}

		// sets the sunlight mesh
		if( eruption.sunlightRenderer != nullptr )
		{
			SunLight& sunlight = _world.GetSingleton<SunLight>();
			eruption.sunlightRenderer->mesh = &sunlight.mesh;
		}

		// initialize the sun point light
		if( eruption.sunlightLight != nullptr )
		{
			eruption.sunlightLight->attenuation[PointLight::LINEAR] = eruption.stateLightAttenuation[WAITING];
			eruption.previousLightAttenuation = eruption.stateLightAttenuation[WAITING];
		}

		// initialize particles tags
		if( eruption.particleEmitter != nullptr )
		{
			const int tagIndex = _world.GetIndex( tag_sunlight_occlusion::s_typeInfo );
			eruption.particleEmitter->tagsSignature[tagIndex] = 1;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Step( EcsWorld& _world, const float _delta )
	{
		SolarEruption& eruption = _world.GetSingleton<SolarEruption>();

		if( !eruption.enabled || _delta == 0.f ) { return; }

		eruption.timer += _delta;

		if( eruption.state == State::WAITING )
		{
			const Game& game = _world.GetSingleton<Game>();
			if( game.frameIndex == eruption.eruptionStartFrame )
			{
				eruption.stateDuration[WAITING] = 0;
			}
		}

		// set particles
		if( eruption.timer > eruption.stateDuration[eruption.state] )
		{
			eruption.previousLightAttenuation = eruption.stateLightAttenuation[eruption.state];

			eruption.state = State( eruption.state + 1 );
			if( eruption.state != State::SIZE )
			{
				eruption.timer = 0.f;
				eruption.particleEmitter->particlesPerSecond = eruption.stateParticlePerSecond[eruption.state];
				eruption.particleEmitter->speed = eruption.stateParticleSpeed[eruption.state];
				eruption.particleEmitter->duration = eruption.stateParticleDuration[eruption.state];
				eruption.particleEmitter->color = eruption.stateParticleColor[eruption.state];
			}
			else
			{
				eruption.state = State::WAITING;
				eruption.stateDuration[WAITING] = std::numeric_limits<float>::max(); // wait forever
			}
		}

		// fading light attenuation
		switch( eruption.state )
		{
		case WAITING:
		case WARMING:
		case COLLAPSING:
		case BACK_TO_NORMAL:
		{
			const float target = eruption.stateLightAttenuation[eruption.state];
			float delta = eruption.timer / eruption.stateDuration[eruption.state]; // between 0-1
			eruption.sunlightLight->attenuation[PointLight::LINEAR] = eruption.previousLightAttenuation + delta * ( target - eruption.previousLightAttenuation );
		}break;
		case EXPODING:
		{
			const float target = eruption.stateLightAttenuation[eruption.state];
			float delta = eruption.timer / eruption.stateDuration[eruption.state]; // between 0-1
			eruption.sunlightLight->attenuation[PointLight::LINEAR] = eruption.previousLightAttenuation + delta * ( target - eruption.previousLightAttenuation );
		}break;
		default:
			assert( false );
			break;
		}
	}

	//================================================================================================================================
	// Gui / serialization helper
	//================================================================================================================================
	std::string StateToString( const SolarEruption::State  _state )
	{
		switch( _state )
		{
		case SolarEruption::WAITING:		return "waiting";		 break;
		case SolarEruption::WARMING:		return "warming";		 break;
		case SolarEruption::COLLAPSING:	return "collapsing";	 break;
		case SolarEruption::EXPODING:		return "exploding";		 break;
		case SolarEruption::BACK_TO_NORMAL:return "back to normal"; break;
		default:			return "ERROR";			 break;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::NetSave( const EcsSingleton& _component, sf::Packet& _packet )
	{
		const SolarEruption& solarEruption = static_cast<const SolarEruption&>( _component );
		_packet << FrameIndexNet( solarEruption.eruptionStartFrame);
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::NetLoad( EcsSingleton& _component, sf::Packet& _packet )
	{
		SolarEruption& solarEruption = static_cast< SolarEruption&>( _component );

		FrameIndexNet eruptionStartFrame;
		_packet >> eruptionStartFrame;

		solarEruption.eruptionStartFrame = eruptionStartFrame;
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Save( const EcsSingleton& _component, Json& _json )
	{
		const SolarEruption& solarEruption = static_cast<const SolarEruption&>( _component );

		Serializable::SaveComponentPtr( _json, "sunlight_material", solarEruption.sunlightMaterial );
		Serializable::SaveComponentPtr( _json, "sunlight_renderer", solarEruption.sunlightRenderer );
		Serializable::SaveComponentPtr( _json, "particle_emitter", solarEruption.particleEmitter );
		Serializable::SaveComponentPtr( _json, "sunlight_light", solarEruption.sunlightLight );

		Serializable::SaveFloat( _json, "cooldown", solarEruption.cooldown );
		Serializable::SaveFloat( _json, "random_cooldown", solarEruption.randomCooldown );
		Serializable::SaveFloat( _json, "damage_per_second", solarEruption.damagePerSecond );

		for( int i = 0; i < SIZE; i++ )
		{
			std::string name = StateToString( State( i ) );

			Serializable::SaveFloat( _json, ( name + "_duration" ).c_str(), solarEruption.stateDuration[i] );
			Serializable::SaveFloat( _json, ( name + "_light_attenuation_linear" ).c_str(), solarEruption.stateLightAttenuation[i] );
			Serializable::SaveFloat( _json, ( name + "_particle_per_second" ).c_str(), solarEruption.stateParticlePerSecond[i] );
			Serializable::SaveFloat( _json, ( name + "_particle_speed" ).c_str(), solarEruption.stateParticleSpeed[i] );
			Serializable::SaveFloat( _json, ( name + "_particle_duration" ).c_str(), solarEruption.stateParticleDuration[i] );
			Serializable::SaveColor( _json, ( name + "_particle_color" ).c_str(), solarEruption.stateParticleColor[i] );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::Load( EcsSingleton& _component, const Json& _json )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );

		Serializable::LoadComponentPtr( _json, "sunlight_material", solarEruption.sunlightMaterial );
		Serializable::LoadComponentPtr( _json, "sunlight_renderer", solarEruption.sunlightRenderer );
		Serializable::LoadComponentPtr( _json, "particle_emitter", solarEruption.particleEmitter );
		Serializable::LoadComponentPtr( _json, "sunlight_light", solarEruption.sunlightLight );

		Serializable::LoadFloat( _json, "cooldown", solarEruption.cooldown );
		Serializable::LoadFloat( _json, "random_cooldown", solarEruption.randomCooldown );
		Serializable::LoadFloat( _json, "damage_per_second", solarEruption.damagePerSecond );

		for( int i = 0; i < SIZE; i++ )
		{
			std::string name = StateToString( State(i) );

			Serializable::LoadFloat( _json, ( name + "_duration" ).c_str(), solarEruption.stateDuration[i] );
			Serializable::LoadFloat( _json, ( name + "_light_attenuation_linear" ).c_str(), solarEruption.stateLightAttenuation[i] );
			Serializable::LoadFloat( _json, ( name + "_particle_per_second" ).c_str(), solarEruption.stateParticlePerSecond[i] );
			Serializable::LoadFloat( _json, ( name + "_particle_speed" ).c_str(), solarEruption.stateParticleSpeed[i] );
			Serializable::LoadFloat( _json, ( name + "_particle_duration" ).c_str(), solarEruption.stateParticleDuration[i] );
			Serializable::LoadColor( _json, ( name + "_particle_color" ).c_str(), solarEruption.stateParticleColor[i] );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void SolarEruption::OnGui( EcsWorld& _world, EcsSingleton& _component )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );

		ImGui::Indent(); ImGui::Indent();
		{
			ImGui::PushItemWidth( 200.f );
			{
				const Game& game = _world.GetSingleton<Game>();
				ImGui::Text( "state: %s", StateToString( solarEruption.state ).c_str() );
				ImGui::Text( "timer: %f", solarEruption.timer );

				const float nextEruptionTime = game.logicDelta * ( solarEruption.eruptionStartFrame - game.frameIndex );
				ImGui::Text( "next eruption: %f", nextEruptionTime );

				ImGui::Spacing();

				// misc
				if( ImGui::CollapsingHeader( "global settings" ) )
				{
					ImGui::Checkbox( "enabled", &solarEruption.enabled );
					ImGui::DragFloat( "cooldown", &solarEruption.cooldown );
					ImGui::DragFloat( "random cooldown", &solarEruption.randomCooldown );
					ImGui::DragFloat( "damage per second", &solarEruption.damagePerSecond, 1.f, 0.f, 100.f );
					if( ImGui::Button( "Eruption" ) ) { solarEruption.stateDuration[WAITING] = 0.f; }
				}

				// states parameters
				for( int i = 0; i < SIZE; i++ )
				{
					std::string name = StateToString( State( i ) );
					if( ImGui::CollapsingHeader( ( "state: " + name ).c_str() ) )
					{
						ImGui::DragFloat( ( "duration##" + name ).c_str(), &solarEruption.stateDuration[i] );
						ImGui::DragFloat( ( "particle per second##" + name ).c_str(), &solarEruption.stateParticlePerSecond[i] );
						ImGui::DragFloat( ( "particle speed##" + name ).c_str(), &solarEruption.stateParticleSpeed[i] );
						ImGui::DragFloat( ( "particle duration##" + name ).c_str(), &solarEruption.stateParticleDuration[i] );
						ImGui::DragFloat( ( "linear light attenuation##" + name ).c_str(), &solarEruption.stateLightAttenuation[i], 0.001f, 0.f, 1.f );
						ImGui::ColorEdit4( ( "state particle color##" + name ).c_str(), solarEruption.stateParticleColor[i].Data(), ImGui::fanColorEditFlags );
					}
				}

				ImGui::Spacing();
			} ImGui::PopItemWidth();

			// components 
			ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
			{
				ImGui::FanComponent<Material>( "sunlight material", solarEruption.sunlightMaterial );
				ImGui::FanComponent<MeshRenderer>( "sunlight renderer", solarEruption.sunlightRenderer );
				ImGui::FanComponent<ParticleEmitter>( "particle emitter", solarEruption.particleEmitter );
				ImGui::FanComponent<PointLight>( "sunlight light", solarEruption.sunlightLight );
			} ImGui::PopItemWidth();

		}
		ImGui::Unindent(); ImGui::Unindent();
	}
}