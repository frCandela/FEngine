#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/actors/fanActor.hpp"

namespace fan
{
	class Mesh;
	class Material;
	class ParticleSystem;
	class MeshRenderer;
	class PlayersManager;
	struct Vertex;

	//================================================================================================================================
	//================================================================================================================================
	class SolarEruption : public Actor
	{
	public:
		void Start() override;
		void Stop() override {}
		void Update( const float _delta ) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::JOYSTICK16; }

		DECLARE_TYPE_INFO( SolarEruption, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

	private:
		Material* m_material;
		MeshRenderer* m_meshRenderer;
		ParticleSystem* m_particleSystem;
		PlayersManager* m_players;

		// State machine
		float   m_eruptionTime = -1.f;
		float	m_eruptionCooldown = 10.f;
		float	m_eruptionRandomCooldown = 10.f;
		float	m_eruptionDamagePerSecond = 10.f;

		float   m_warmingTime = 2.f;
		float   m_warmingMaxParticles = 10.f;
		float   m_waitingTime = 2.f;
		float   m_waitingMaxParticles = 100.f;
		float   m_explosionTime = 2.f;
		float   m_explosionFadeinTime = 0.25f;
		float   m_explosionMaxParticles = 500.f;
		float   m_explosionParticlesSpeed = 50.f;
		float   m_backToNormalTime = 0.25f;

		float m_timeBeforeEruption = 0;

		float m_collapseAlpha = 0.8f;
		Color m_baseColor = Color::Yellow;
		Color m_explositonColor = Color::Red;

		std::default_random_engine			  m_generator;
		std::uniform_real_distribution<float> m_distribution;

		enum State { NONE, COLLAPSING, WAITING, EXPODING, BACK_TO_NORMAL };
		State m_state = State::NONE;

		void UpdateStateMachine( const float _delta );
		void StartEruption();
	};
}