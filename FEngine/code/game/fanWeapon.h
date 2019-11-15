#pragma once

#include "scene/actors/fanActor.h"

namespace fan
{
	class Rigidbody;
	class WithEnergy;

	//================================================================================================================================
	//================================================================================================================================
	class Weapon : public Actor
	{
	public:
		void Start() override;
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::GAME_MANAGER; }

		DECLARE_TYPE_INFO( Weapon, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		// ref
		WithEnergy * m_energy;

		// Bullets parameters
		float m_lifeTime = 1.f;
		float m_scale = 0.2f;
		float m_speed = 0.2f;
		float m_bulletsPerSecond = 10.f;
		float m_bulletEnergyCost = 1.f;

		// explosion parameters
		float m_explosionTime = 0.15f;
		float m_exposionSpeed = 2.f;
		int m_particlesPerExplosion = 3;

		btVector3 m_offset ;

		// Accumulator
		float m_bulletsAccumulator = 0.f;

		// for particles
		std::default_random_engine			  m_generator;
		std::uniform_real_distribution<float> m_distribution;

		void OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold );
		void CreateExplosion( const btVector3 _point );
		void FireBullet();
	};
}