#include "fanGlobalIncludes.h"
#include "game/fanWeapon.h"

#include "core/time/fanTime.h"
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

	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnDetach()
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::Update( const float /*_delta*/ )
	{
		SCOPED_PROFILE( weapon_update )
		Transform * thisTransform = m_gameobject->GetTransform();

		const float fire = Input::Get().Manager().GetAxis( "fire" );
		if ( fire > 0 )
		{


			Gameobject * bulletGO = m_gameobject->GetScene()->CreateGameobject("bullet", m_gameobject );
			Model * model = bulletGO->AddComponent<Model>();
			Material * material = bulletGO->AddComponent<Material>();
			Rigidbody * rb = bulletGO->AddComponent<Rigidbody>();
			SphereShape * collider = bulletGO->AddComponent<SphereShape>();
			Transform * transform = bulletGO->GetTransform();

			model->SetPath(GlobalValues::s_meshSphere);
			model->SetAutoUpdateHull(false);

			material->SetTexturePath( GlobalValues::s_textureWhite );
			collider->SetRadius( m_scale );
			transform->SetScale(btVector3( m_scale, m_scale, m_scale ));
			transform->SetPosition( thisTransform->GetPosition() + thisTransform->TransformDirection( m_offset) );
			rb->SetVelocity( m_speed * thisTransform->Forward() );

		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Weapon::OnGui()
	{
		ImGui::DragFloat("scale##wepoffset", &m_scale, 0.05f, 0.f, 1.f );
		ImGui::DragFloat( "speed##wepspeed", &m_speed, 0.1f, 0.f, 100.f );
		ImGui::DragFloat3( "offset##wepspeed", &m_offset[0]);
		ImGui::DragFloat( "lifeTime##wepspeed", &m_lifeTime, 1.f, 0.f, 100.f );
		
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

		return true;
	}


}