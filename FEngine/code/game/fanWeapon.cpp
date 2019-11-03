#include "fanGlobalIncludes.h"
#include "game/fanWeapon.h"

#include "core/time/fanTime.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/fanMaterial.h"
#include "scene/components/fanRigidbody.h"
#include "scene/components/fanModel.h"
#include "scene/components/fanSphereShape.h"
#include "scene/fanGameobject.h"
#include "core/input/fanInput.h"
#include "core/input/fanInputManager.h"
#include "core/input/fanMouse.h"
#include "core/time/fanProfiler.h"
#include "renderer/fanRenderer.h"


namespace fan
{
	REGISTER_EDITOR_COMPONENT( Weapon )
	REGISTER_TYPE_INFO( Weapon )

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Start()
	{}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnAttach()
	{
		Actor::OnAttach();
		m_distribution = std::uniform_real_distribution<float> ( 0.f, 1.f );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Update( const float /*_delta*/ ){}

	void Weapon::LateUpdate( const float /*_delta*/ )  
	{
		SCOPED_PROFILE( weapon_update )
		Transform * thisTransform = m_gameobject->GetTransform();

		const float fire = Input::Get().Manager().GetAxis( "fire" );
		if ( fire > 0 )
		for (int bulletIndex = 0; bulletIndex < m_bulletsPerFrame; bulletIndex++)
		{


			Gameobject * bulletGO = m_gameobject->GetScene()->CreateGameobject("bullet", m_gameobject );
			Model * model = bulletGO->AddComponent<Model>();
			Material * material = bulletGO->AddComponent<Material>();
			Rigidbody * rb = bulletGO->AddComponent<Rigidbody>();			
			rb->onContactStarted.Connect(&Weapon::OnBulletContact, this );

			SphereShape * collider = bulletGO->AddComponent<SphereShape>();
			Transform * transform = bulletGO->GetTransform();

			bulletGO->AddFlag(ecsFlags::NO_AABB_UPDATE );
			bulletGO->AddEcsComponent<ecsBullet>()->Init(m_lifeTime);
			model->SetPath(GlobalValues::s_meshSphere);
			material->SetTexturePath( GlobalValues::s_textureWhite );
			collider->SetRadius( m_scale );
			transform->SetScale(btVector3( m_scale, m_scale, m_scale ));
			transform->SetPosition( thisTransform->GetPosition() + thisTransform->TransformDirection(m_offset) ); 
			
			rb->SetMass(1.f); 

			Rigidbody * myRb = m_gameobject->GetComponent<Rigidbody>();
			if ( myRb )
			{
				rb->GetBtBody()->setIgnoreCollisionCheck( myRb->GetBtBody(), true );
			}

			
			rb->SetVelocity( myRb->GetVelocity() + m_speed * thisTransform->Forward() );
			  
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnBulletContact( Rigidbody* _other, btPersistentManifold* const& _manifold )
	{
		
		Rigidbody * rb0 = static_cast<Rigidbody*> ( _manifold->getBody0()->getUserPointer() );
 		Rigidbody * rb1 = static_cast<Rigidbody*> ( _manifold->getBody1()->getUserPointer() );
		Rigidbody * bulletRb = _other == rb0 ? rb1 : rb0;

		m_gameobject->GetScene()->DeleteGameobject( bulletRb->GetGameobject() );
		CreateExplosion( bulletRb->GetGameobject()->GetTransform()->GetPosition());

	}

	//================================================================================================================================
	//================================================================================================================================
	void  Weapon::CreateExplosion( const btVector3 _point )
	{
		for (int particleIndex = 0; particleIndex < m_particlesPerExplosion ; particleIndex++)
		{		
			EcsManager * ecs = m_gameobject->GetScene()->GetEcsManager();
			ecsEntity entity = ecs->CreateEntity();
			ecsPosition & position = ecs->AddComponent<ecsPosition>( entity );
			ecs->AddComponent<ecsRotation>( entity ).Init();
			ecsMovement & movement = ecs->AddComponent<ecsMovement>( entity );
			ecsParticle & particle = ecs->AddComponent<ecsParticle>( entity );

			movement.speed = btVector3( m_distribution( m_generator ), m_distribution( m_generator ), m_distribution( m_generator ) ) - btVector3( 0.5f, 0.5f, 0.5f );
			movement.speed.normalize();
			movement.speed *= m_distribution( m_generator ) * m_exposionSpeed;
			position.position = _point;
			particle.durationLeft = m_explosionTime;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui()
	{
		ImGui::DragFloat("scale ##wepoffset", &m_scale, 0.05f, 0.f, 1.f );
		ImGui::DragFloat( "speed ##wepspeed", &m_speed, 0.1f, 0.f, 100.f );
		ImGui::DragFloat3( "offset ##wepspeed", &m_offset[0]);
		ImGui::DragFloat( "life time ##wepspeed", &m_lifeTime, 1.f, 0.f, 100.f );
		ImGui::DragInt( "bulletsPerFrame ##wep__bulletsPerFrame", &m_bulletsPerFrame);
		ImGui::Spacing();
		ImGui::DragFloat( "explosion time ##wep_exposionTime",   &m_explosionTime, 0.05f, 0.f, 10.f );
		ImGui::DragFloat( "explosion speed ##wep_exposionSpeed", &m_exposionSpeed, 0.5f , 0.f, 100.f );	
		ImGui::DragInt( "particles per explosion ##wep_particlesPerExplosion", &m_particlesPerExplosion );
		
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Weapon::Save( Json & _json ) const
	{
		Actor::Save( _json );
		SaveFloat( _json, "scale", m_scale );  
		SaveFloat( _json, "speed", m_speed );
		SaveVec3( _json, "offset", m_offset );
		SaveFloat( _json, "lifeTime", m_lifeTime );
		SaveInt( _json, "bulletsPerFrame", m_bulletsPerFrame );
		SaveFloat( _json, "exposion_speed", m_exposionSpeed );
		SaveFloat( _json, "explosion_time", m_explosionTime );
		SaveInt( _json, "particlesPerExplosion", m_particlesPerExplosion );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool Weapon::Load( Json & _json )
	{
		Actor::Load( _json );
		LoadFloat( _json,"scale", m_scale );
		LoadFloat( _json,"speed", m_speed );
		LoadVec3( _json, "offset", m_offset );
		LoadFloat( _json, "lifeTime", m_lifeTime );
		LoadInt( _json, "bulletsPerFrame", m_bulletsPerFrame );
		LoadFloat( _json, "exposion_speed", m_exposionSpeed );
		LoadFloat( _json, "explosion_time", m_explosionTime );
		LoadInt( _json, "particlesPerExplosion", m_particlesPerExplosion );
		return true;
	}


}