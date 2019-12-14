#include "fanGlobalIncludes.h"
#include "scene/components/fanFollowTransform.h"

#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_TYPE_INFO( FollowTransform, TypeInfo::Flags::EDITOR_COMPONENT, "other/" )


	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnAttach()
	{
		Actor::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnDetach()		
	{
		Actor::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Start()
	{
		REQUIRE_TRUE( *m_followedTransform != nullptr, "SpaceshipUI: missing reference" )

		if( *m_followedTransform != nullptr )
		{
			m_offset = m_followedTransform->GetPosition() - m_gameobject->GetTransform().GetPosition();
		}	
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::Update( const float /*_delta*/ )
	{
		UpdatePosition();			
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::UpdatePosition()
	{
		Transform & transform = m_gameobject->GetTransform();

		btVector3 offset = m_followedTransform->GetPosition() - transform.GetPosition() - m_offset;

		if ( !offset.isZero() )
		{
			transform.SetPosition( transform.GetPosition() + offset );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void FollowTransform::OnGui()
	{
		ImGui::FanComponent("followed transform", &m_followedTransform );
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool FollowTransform::Save( Json & _json ) const
	{
		Actor::Save( _json );
		SaveComponentPtr(_json, "followed_transform", m_followedTransform );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool FollowTransform::Load( const Json & _json )
	{
		Actor::Load( _json );
		LoadComponentPtr(_json, "followed_transform", m_followedTransform );
		return true;
	}
}