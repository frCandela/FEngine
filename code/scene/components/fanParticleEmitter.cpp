#include "scene/components/fanParticleEmitter.hpp"
#include "render/fanRenderSerializable.hpp"
#include "editor/fanModals.hpp"

namespace fan
{
	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::SetInfo( EcsComponentInfo& _info )
	{
		_info.load        = &ParticleEmitter::Load;
		_info.save        = &ParticleEmitter::Save;
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );
		emitter.mEnabled            = true;
		emitter.mParticlesPerSecond = 100.f;
		emitter.mSpeed              = 1.f;
		emitter.mDuration           = 2.f;
		emitter.mColor              = Color::sMagenta;
		emitter.mTimeAccumulator    = 0.f;
		emitter.mTagsSignature      = 0;
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Save( const EcsComponent& _component, Json& _json )
	{
		const ParticleEmitter& emitter = static_cast<const ParticleEmitter&>( _component );

		Serializable::SaveBool( _json, "enabled", emitter.mEnabled );
		Serializable::SaveFloat( _json, "particles_per_second", emitter.mParticlesPerSecond );
		Serializable::SaveFloat( _json, "speed", emitter.mSpeed );
		Serializable::SaveFloat( _json, "duration", emitter.mDuration );
		Serializable::SaveColor( _json, "color", emitter.mColor );
	}

	//========================================================================================================
	//========================================================================================================
	void ParticleEmitter::Load( EcsComponent& _component, const Json& _json )
	{
		ParticleEmitter& emitter = static_cast<ParticleEmitter&>( _component );

		Serializable::LoadBool( _json, "enabled", emitter.mEnabled );
		Serializable::LoadFloat( _json, "particles_per_second", emitter.mParticlesPerSecond );
		Serializable::LoadFloat( _json, "speed", emitter.mSpeed );
		Serializable::LoadFloat( _json, "duration", emitter.mDuration );
		Serializable::LoadColor( _json, "color", emitter.mColor );
	}
}