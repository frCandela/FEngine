#pragma once

#include "scene/actors/fanActor.h"

namespace fan {
	class EcsManager;

	//================================================================================================================================
	//================================================================================================================================
	class ParticleSystem : public Actor {
	public:
		void Start() override;
		void Update( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::PARTICLES; }

		DECLARE_EDITOR_COMPONENT( ParticleSystem )
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

		int		m_particlesPerSecond = 1;
		float	m_speed = 1.f;
		float	m_duration = 2.f;
		btVector3 m_offset = btVector3::Zero();

		float	m_timeAccumulator = 0.f;

	};
}