#include "fanGlobalIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/components/fanComponent.h"
#include "scene/components/fanRigidbody.h"
#include "scene/fanScene.h"	
#include "core/fanSignal.h"
#include "ecs/fanECSManager.h"

namespace fan
{
	REGISTER_TYPE_INFO(Transform, TypeInfo::Flags::NONE, "")

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnAttach() {
 		Component::OnAttach();

		ecsTranform** tmpTransform = &const_cast<ecsTranform*>( m_transform );
		*tmpTransform = m_gameobject->AddEcsComponent<ecsTranform>();
		m_transform->Init();

		ecsScaling ** tmpScaling = &const_cast<ecsScaling*>( m_scale );
		*tmpScaling = m_gameobject->AddEcsComponent<ecsScaling>();
		m_scale->Init();
 
 		SetRemovable(false);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnDetach() {
		Component::OnDetach();

		m_gameobject->RemoveEcsComponent<ecsTranform>();
		m_gameobject->RemoveEcsComponent<ecsScaling>();
	}

	//================================================================================================================================
	// Getters
	//================================================================================================================================
	const btTransform&	Transform::GetBtTransform() const	{ return m_transform->transform; }
	btVector3			Transform::GetPosition() const		{ return m_transform->transform.getOrigin(); }
	btVector3			Transform::GetScale() const			{ return m_scale->scale; }
	btQuaternion		Transform::GetRotationQuat() const { return m_transform->transform.getRotation(); }
	glm::mat4			Transform::GetNormalMatrix() const { return glm::transpose( glm::inverse( GetModelMatrix() ) ); }
	btVector3			Transform::Left() const { return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Left(); }
	btVector3			Transform::Forward() const { return btTransform( GetRotationQuat() ) * btVector3::Forward(); }
	btVector3			Transform::Up() const { return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Up(); }

	//================================================================================================================================
	//================================================================================================================================
	void Transform::LookAt( const btVector3& _target, const btVector3& _up ) {

		// Move rigidbody
		Rigidbody * rb = m_gameobject->GetComponent<Rigidbody>();
		if ( rb )
		{
			if ( rb->IsStatic() ){return;}
			rb->SetTransform( m_transform->transform );
		}

		btVector3 forward = _target - m_transform->transform.getOrigin();
		forward.normalize();
		btVector3 left = _up.cross( forward );
		left.normalize();

		m_transform->transform.setBasis( btMatrix3x3 (	left[0], _up[0], forward[0],
														left[1], _up[1], forward[1],
														left[2], _up[2], forward[2] ) );
		m_gameobject->SetFlags( m_gameobject->GetFlags() | Gameobject::Flag::OUTDATED_AABB );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition(btVector3 _newPosition) {
		// Move rigidbody
		Rigidbody * rb = m_gameobject->GetComponent<Rigidbody>();
		if ( rb )
		{
			if ( rb->IsStatic() ) { return; }
			rb->SetTransform( m_transform->transform );
		}

		if ( m_transform->transform.getOrigin() != _newPosition) {
			m_transform->transform.setOrigin( _newPosition);
			m_gameobject->SetFlags( m_gameobject->GetFlags() | Gameobject::Flag::OUTDATED_AABB );
 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetScale(btVector3 _newScale) {
		if (m_scale->scale != _newScale) {
			m_scale->scale = _newScale;
			m_gameobject->SetFlags( m_gameobject->GetFlags() | Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationEuler(const btVector3 _rotation) {
		btQuaternion quat;
		quat.setEulerZYX(btRadians(_rotation.z()), btRadians(_rotation.y()), btRadians(_rotation.x()));
		SetRotationQuat(quat);
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::GetRotationEuler() const {
		btVector3 euler;
		m_transform->transform.getRotation().getEulerZYX(euler[2], euler[1], euler[0]);
		return btDegrees3(euler);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat(const btQuaternion _rotation) {
		// Move rigidbody
		Rigidbody * rb = m_gameobject->GetComponent<Rigidbody>();
		if ( rb )
		{
			if ( rb->IsStatic() ) { return; }
			rb->SetTransform( m_transform->transform );
		}

		if ( m_transform->transform.getRotation() != _rotation) {
			m_transform->transform.setRotation( _rotation);
			m_gameobject->SetFlags( m_gameobject->GetFlags() | Gameobject::Flag::OUTDATED_AABB );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform::GetModelMatrix() const {
		glm::vec3 position =	ToGLM( m_transform->transform.getOrigin());
		glm::vec3 scale =		ToGLM( m_scale->scale );
		glm::quat rotation =	ToGLM( m_transform->transform.getRotation());

		return glm::translate(glm::mat4(1.f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), scale);
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::TransformPoint(const btVector3 _point) const {
		const btTransform transform = GetBtTransform();
		btVector3 transformedPoint = transform * ( m_scale->scale * _point);
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::InverseTransformPoint(const btVector3 _point) const {
		const btVector3 invertScale(1.f / m_scale->scale[0], 1.f / m_scale->scale[1], 1.f / m_scale->scale[2]);
		const btTransform transform = GetBtTransform();
		btVector3 transformedPoint = invertScale * (transform.inverse()*_point);
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::TransformDirection(const btVector3 _point) const {
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = transform * _point;
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::InverseTransformDirection(const btVector3 _point) const {
		const btVector3 invertScale(1.f / m_scale->scale[0], 1.f / m_scale->scale[1], 1.f / m_scale->scale[2]);
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = invertScale * (transform.inverse()*_point);
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnGui()
	{
		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{

			// Position
			if ( ImGui::Button( "##TransPos" ) )
			{
				SetPosition( btVector3( 0, 0, 0 ) );
			} ImGui::SameLine();
			float posBuffer[3] = { GetPosition().x(), GetPosition().y(), GetPosition().z() };
			if ( ImGui::DragFloat3( "position", posBuffer, 0.1f ) )
			{
				SetPosition( btVector3( posBuffer[0], posBuffer[1], posBuffer[2] ) );
			}

			// rotation
			if ( ImGui::Button( "##TransRot" ) )
			{
				SetRotationQuat( btQuaternion::getIdentity() );
			} ImGui::SameLine();
			const btVector3 rot = GetRotationEuler();
			float bufferAngles[3] = { rot.x(),rot.y(),rot.z() };
			if ( ImGui::DragFloat3( "rotation", bufferAngles, 0.1f ) )
			{
				SetRotationEuler( btVector3( bufferAngles[0], bufferAngles[1], bufferAngles[2] ) );
			}

			// Scale
			if ( ImGui::Button( "##TransScale" ) )
			{
				SetScale( btVector3( 1, 1, 1 ) );
			} ImGui::SameLine();
			btVector3 scale = GetScale();
			if ( ImGui::DragFloat3( "scale", &scale[0], 0.1f ) )
			{
				SetScale( scale );
			}
		}
		ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Transform::Load( const Json & _json ) {
		btVector3 tmpVec;
		btQuaternion tmpQuat;

		LoadVec3( _json, "position", tmpVec );
		LoadQuat( _json, "rotation", tmpQuat );
		LoadVec3( _json, "scale", m_scale->scale );

		m_transform->transform.setOrigin( tmpVec );
		m_transform->transform.setRotation( tmpQuat );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Transform::Save( Json & _json ) const {

		SaveVec3( _json, "position", m_transform->transform.getOrigin() );
		SaveQuat( _json, "rotation", m_transform->transform.getRotation() );
		SaveVec3( _json, "scale", m_scale->scale );


		Component::Save( _json );
				
		return true;
	}	
}