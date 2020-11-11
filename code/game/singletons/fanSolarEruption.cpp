#include "game/singletons/fanSolarEruption.hpp"

#include "core/fanDebug.hpp"
#include "core/fanRandom.hpp"
#include "network/fanPacket.hpp"
#include "network/singletons/fanTime.hpp"
#include "network/singletons/fanSpawnManager.hpp"
#include "network/systems/fanHostReplication.hpp"
#include "game/spawn/fanSpawnSolarEruption.hpp"
#include "game/singletons/fanSunLight.hpp"
#include "game/fanGameTags.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void SolarEruption::SetInfo( EcsSingletonInfo& _info )
	{
		_info.save   = &SolarEruption::Save;
		_info.load   = &SolarEruption::Load;
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::Init( EcsWorld& _world, EcsSingleton& _component )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );

		solarEruption.mSunlightMaterial.Init( _world );
		solarEruption.mSunlightRenderer.Init( _world );
		solarEruption.mParticleEmitter.Init( _world );
		solarEruption.mSunlightLight.Init( _world );

		solarEruption.mSpawnFrame      = 0;
		solarEruption.mTimer           = 0;
		solarEruption.mEnabled         = false;
		solarEruption.mState           = State::WAITING;
		solarEruption.mCooldown        = 5.f;
		solarEruption.mRandomCooldown  = 1.f;
		solarEruption.mDamagePerSecond = 10.f;

		for( int i = 0; i < SIZE; i++ )
		{
			solarEruption.mStateDuration[i]          = 2.f;
			solarEruption.mStateParticlePerSecond[i] = 100.f;
			solarEruption.mStateParticleSpeed[i]     = 3.f;
			solarEruption.mStateParticleDuration[i]  = 3.f;
			solarEruption.mStateParticleColor[i]     = Color::sMagenta;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::Start( EcsWorld& _world )
	{
		SolarEruption& eruption = _world.GetSingleton<SolarEruption>();

		eruption.mEnabled = true;
		eruption.mStateDuration[WAITING] = std::numeric_limits<float>::max(); // wait forever

		// pointers check
		if( eruption.mSunlightMaterial == nullptr
			|| eruption.mSunlightRenderer == nullptr
			|| eruption.mParticleEmitter == nullptr
			|| eruption.mSunlightLight == nullptr )
		{
			Debug::Error() << "SolarEruption : missing component references." << Debug::Endl();
			eruption.mEnabled = false;
		}

		// sets the sunlight mesh
		if( eruption.mSunlightRenderer != nullptr )
		{
			SunLight& sunlight = _world.GetSingleton<SunLight>();
			eruption.mSunlightRenderer->mMesh = sunlight.mMesh;
		}

		// initialize the sun point light
		if( eruption.mSunlightLight != nullptr )
		{
			eruption.mSunlightLight->mAttenuation[PointLight::Linear] = eruption.mStateLightAttenuation[WAITING];
			eruption.mPreviousLightAttenuation = eruption.mStateLightAttenuation[WAITING];
		}

		// initialize particles tags
		if( eruption.mParticleEmitter != nullptr )
		{
			const int tagIndex = _world.GetIndex( TagSunlightOcclusion::Info::sType );
			eruption.mParticleEmitter->mTagsSignature[tagIndex] = 1;
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::SpawnEruptionNow()
	{
        mState = State::WAITING;
        mStateDuration[WAITING] = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::ScheduleNextEruption( EcsWorld& _world )
	{
		// spawns the next eruption & replicates it on all hosts
		SpawnManager& spawnManager = _world.GetSingleton<SpawnManager>();
		const Time& time = _world.GetSingleton<Time>();

		const float delay = mCooldown + Random::Float() * mRandomCooldown;
        mSpawnFrame = time.mFrameIndex + (FrameIndex)( delay / time.mLogicDelta );

		SpawnInfo spawnInfo = spawn::SpawnSolarEruption::GenerateInfo( mSpawnFrame );
		spawnManager.spawns.push_back( spawnInfo );
        _world.Run<SReplicateOnAllHosts>( ClientRPC::RPCSpawn( spawnInfo ),
                                          HostReplication::ResendUntilReplicated );
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::Step( EcsWorld& _world, const float _delta )
	{
		SolarEruption& eruption = _world.GetSingleton<SolarEruption>();

		if( !eruption.mEnabled || _delta == 0.f ) { return; }

		eruption.mTimer += _delta;

		// set particles
		if( eruption.mTimer > eruption.mStateDuration[eruption.mState] )
		{
			eruption.mPreviousLightAttenuation = eruption.mStateLightAttenuation[eruption.mState];

			eruption.mState = State( eruption.mState + 1 );
			if( eruption.mState != State::SIZE )
			{
				eruption.mTimer                                = 0.f;
				eruption.mParticleEmitter->mParticlesPerSecond = eruption.mStateParticlePerSecond[eruption.mState];
				eruption.mParticleEmitter->mSpeed              = eruption.mStateParticleSpeed[eruption.mState];
				eruption.mParticleEmitter->mDuration           = eruption.mStateParticleDuration[eruption.mState];
				eruption.mParticleEmitter->mColor              = eruption.mStateParticleColor[eruption.mState];
			}
			else
			{
				eruption.mState = State::WAITING;
				eruption.mStateDuration[WAITING] = std::numeric_limits<float>::max(); // wait forever
			}
		}

		// fading light attenuation
		switch( eruption.mState )
		{
		case WAITING:
		case WARMING:
		case COLLAPSING:
		case BACK_TO_NORMAL:
		{
			const float target = eruption.mStateLightAttenuation[eruption.mState];
			float delta = eruption.mTimer / eruption.mStateDuration[eruption.mState]; // between 0-1
			eruption.mSunlightLight->mAttenuation[PointLight::Linear] =
                    eruption.mPreviousLightAttenuation + delta * ( target - eruption.mPreviousLightAttenuation );
		}break;
		case EXPODING:
		{
			const float target = eruption.mStateLightAttenuation[eruption.mState];
			float delta = eruption.mTimer / eruption.mStateDuration[eruption.mState]; // between 0-1
			eruption.mSunlightLight->mAttenuation[PointLight::Linear] =
                    eruption.mPreviousLightAttenuation + delta * ( target - eruption.mPreviousLightAttenuation );
		}break;
		default:
            fanAssert( false );
			break;
		}
	}

	//========================================================================================================
	// Gui / serialization helper
	//========================================================================================================
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

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::Save( const EcsSingleton& _component, Json& _json )
	{
		const SolarEruption& solarEruption = static_cast<const SolarEruption&>( _component );

		Serializable::SaveComponentPtr( _json, "sunlight_material", solarEruption.mSunlightMaterial );
		Serializable::SaveComponentPtr( _json, "sunlight_renderer", solarEruption.mSunlightRenderer );
		Serializable::SaveComponentPtr( _json, "particle_emitter", solarEruption.mParticleEmitter );
		Serializable::SaveComponentPtr( _json, "sunlight_light", solarEruption.mSunlightLight );

		Serializable::SaveFloat( _json, "cooldown", solarEruption.mCooldown );
		Serializable::SaveFloat( _json, "random_cooldown", solarEruption.mRandomCooldown );
		Serializable::SaveFloat( _json, "damage_per_second", solarEruption.mDamagePerSecond );

		for( int i = 0; i < SIZE; i++ )
		{
			std::string name = StateToString( State( i ) );

            Serializable::SaveFloat( _json, ( name + "_duration" ).c_str(), solarEruption.mStateDuration[i] );
            Serializable::SaveFloat( _json,
                                     ( name + "_light_attenuation_linear" ).c_str(),
                                     solarEruption.mStateLightAttenuation[i] );
            Serializable::SaveFloat( _json,
                                     ( name + "_particle_per_second" ).c_str(),
                                     solarEruption.mStateParticlePerSecond[i] );
            Serializable::SaveFloat( _json,
                                     ( name + "_particle_speed" ).c_str(),
                                     solarEruption.mStateParticleSpeed[i] );
            Serializable::SaveFloat( _json,
                                     ( name + "_particle_duration" ).c_str(),
                                     solarEruption.mStateParticleDuration[i] );
            Serializable::SaveColor( _json,
                                     ( name + "_particle_color" ).c_str(),
                                     solarEruption.mStateParticleColor[i] );
		}
	}

	//========================================================================================================
	//========================================================================================================
	void SolarEruption::Load( EcsSingleton& _component, const Json& _json )
	{
		SolarEruption& solarEruption = static_cast<SolarEruption&>( _component );

		Serializable::LoadComponentPtr( _json, "sunlight_material", solarEruption.mSunlightMaterial );
		Serializable::LoadComponentPtr( _json, "sunlight_renderer", solarEruption.mSunlightRenderer );
		Serializable::LoadComponentPtr( _json, "particle_emitter", solarEruption.mParticleEmitter );
		Serializable::LoadComponentPtr( _json, "sunlight_light", solarEruption.mSunlightLight );

		Serializable::LoadFloat( _json, "cooldown", solarEruption.mCooldown );
		Serializable::LoadFloat( _json, "random_cooldown", solarEruption.mRandomCooldown );
		Serializable::LoadFloat( _json, "damage_per_second", solarEruption.mDamagePerSecond );

		for( int i = 0; i < SIZE; i++ )
		{
			std::string name = StateToString( State(i) );

            Serializable::LoadFloat( _json, ( name + "_duration" ).c_str(), solarEruption.mStateDuration[i] );
            Serializable::LoadFloat( _json,
                                     ( name + "_light_attenuation_linear" ).c_str(),
                                     solarEruption.mStateLightAttenuation[i] );
            Serializable::LoadFloat( _json,
                                     ( name + "_particle_per_second" ).c_str(),
                                     solarEruption.mStateParticlePerSecond[i] );
            Serializable::LoadFloat( _json,
                                     ( name + "_particle_speed" ).c_str(),
                                     solarEruption.mStateParticleSpeed[i] );
            Serializable::LoadFloat( _json,
                                     ( name + "_particle_duration" ).c_str(),
                                     solarEruption.mStateParticleDuration[i] );
            Serializable::LoadColor( _json,
                                     ( name + "_particle_color" ).c_str(),
                                     solarEruption.mStateParticleColor[i] );
		}
	}
}