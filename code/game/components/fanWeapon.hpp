#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "ecs/fanComponent.hpp"

namespace fan
{
	struct ComponentInfo;

	//================================================================================================================================
	// fires bullets
	// is placed on the spaceship on the player 
	//================================================================================================================================
	struct Weapon : public Component
	{
		DECLARE_COMPONENT( Weapon )
	public:
		static void SetInfo( ComponentInfo& _info );
		static void Init( Component& _component );
		static void OnGui( Component& _component );
		static void Save( const Component& _component, Json& _json );
		static void Load( Component& _component, const Json& _json );


		// Bullets parameters		( @todo put some of this in the bullet prefab)
		float bulletLifeTime;
		float scale;
		float bulletSpeed;
		float bulletsPerSecond;
		float bulletEnergyCost;
		float bulletDamage;
		btVector3 originOffset;

		// explosion parameters
		float explosionTime;
		float exposionSpeed;
		int particlesPerExplosion;


		// time accumulator
		float bulletsAccumulator;

		// PrefabPtr m_bulletPrefab;
		// WithEnergy* m_energy;
// 		PlayerInput* m_input;

//  	void OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold );
// 		void CreateExplosion( const btVector3 _point );
	};
	static constexpr size_t sizeof_weapon = sizeof( Weapon );
}