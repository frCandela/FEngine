#pragma  once

#include "core/ecs/fanEcsComponent.hpp"
#include "render/fanRenderResourcePtr.hpp"

namespace fan
{
	//========================================================================================================
	// Emits particles 
	// timeAccumulator is used to keep track of time to spawn particles with proper timing
	//========================================================================================================
	struct ParticleEmitter : public EcsComponent
	{
		ECS_COMPONENT( ParticleEmitter )
		static void SetInfo( EcsComponentInfo& _info );
		static void Init( EcsWorld& _world, EcsEntity _entity, EcsComponent& _component );
		static void OnGui( EcsWorld& _world, EcsEntity _entityID, EcsComponent& _component );
		static void Save( const EcsComponent& _component, Json& _json );
		static void Load( EcsComponent& _component, const Json& _json );

		bool         mEnabled;
		float        mParticlesPerSecond;
		float        mSpeed;
		float        mDuration;				// lifetime of a particle in seconds
		float        mTimeAccumulator;
		Color        mColor;
		EcsSignature mTagsSignature;		// tags added to the particles during instantiation
	};
}