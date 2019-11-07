#pragma once

#include "scene/actors/fanActor.h"

namespace fan {
	class Mesh;
	class Material;
	class ParticleSystem;
	class Model;
	struct Vertex;

	//================================================================================================================================
	//================================================================================================================================
	class SolarEruption : public Actor {
	private:
		//================================================================
		//================================================================
		struct OrientedSegment
		{
			enum OpenSide { RIGHT = 1, LEFT = 2, BOTH = RIGHT | LEFT };

			btVector3 direction;
			OpenSide  openSide;
			float norm;
		};

	public:
		void Start() override;
		void Update(const float _delta) override;
		void LateUpdate( const float /*_delta*/ ) override {}

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_EDITOR_COMPONENT( SolarEruption )
		DECLARE_TYPE_INFO( SolarEruption, Actor );
	protected:

		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		Material * m_material;
		Model * m_model;
		ParticleSystem * m_particleSystem;

		// State machine
		float   m_eruptionTime = -1.f;

		float   m_warmingTime = 2.f;
		float   m_warmingMaxParticles = 10.f;
		float   m_waitingTime = 2.f;
		float   m_waitingMaxParticles = 100.f;
		float   m_explosionTime = 2.f;
		float   m_explosionFadeinTime = 0.25f;
		float   m_explosionMaxParticles = 500.f;
		float   m_explosionParticlesSpeed = 50.f;
		float   m_backToNormalTime = 0.25f;

		Color m_baseColor		= Color::Yellow;
		Color m_explositonColor = Color::Red;

		enum State{ NONE, COLLAPSING, WAITING, EXPODING, BACK_TO_NORMAL };
		State m_state = State::NONE;

		void UpdateStateMachine( const float _delta );
	};
}