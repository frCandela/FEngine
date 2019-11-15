#pragma once

#include "scene/actors/fanActor.h"
#include "scene/fanRessourcePtr.h"

namespace fan {
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================
	class ParticleSystem : public Actor {
	public:
		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::PARTICLES; }

		void SetParticlesPerSecond( const int _pps )	{ m_particlesPerSecond = _pps; }
		void SetSpeedParticles( const float _speed )	{ m_speed = _speed; }
		void SetSpawnOffset( const btVector3 _offset )	{ m_offset = _offset; }

		DECLARE_TYPE_INFO( ParticleSystem, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		EcsManager * m_ecsManager = nullptr;
		std::default_random_engine			  m_generator;
		std::uniform_real_distribution<float> m_distribution;

		int			m_particlesPerSecond = 1;
		float		m_speed = 1.f;
		float		m_duration = 2.f;
		btVector3	m_offset = btVector3::Zero();
		GameobjectPtr m_origin;
		Color		m_color = Color::White;

		float	m_timeAccumulator = 0.f;

	};
}