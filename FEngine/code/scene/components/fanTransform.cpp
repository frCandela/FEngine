#include "fanGlobalIncludes.h"

#include "scene/components/fanTransform.h"
#include "scene/components/fanComponent.h"
#include "scene/fanGameobject.h"
#include "scene/fanScene.h"	
#include "core/fanSignal.h"
#include "ecs/fanECSManager.h"

namespace fan
{
	REGISTER_TYPE_INFO(Transform)

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnAttach() {
 		Component::OnAttach();

		m_gameobject->AddEcsComponent<ecsTranform>()->Init();
		m_gameobject->AddEcsComponent<ecsScaling>()->Init();
 
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
	btTransform	Transform::GetBtTransform() const	{ return btTransform(GetEcsTransform()->transform); }
	btVector3 Transform::GetPosition() const		{ return GetEcsTransform()->transform.getOrigin(); }
	btVector3 Transform::GetScale() const			{ return GetEcsScale()->scale; }
	btQuaternion Transform::GetRotationQuat() const { return GetEcsTransform()->transform.getRotation(); }
	glm::mat4 Transform::GetNormalMatrix() const	{ return glm::transpose( glm::inverse( GetModelMatrix() ) ); }
	btVector3 Transform::Left() const				{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Left(); }
	btVector3 Transform::Forward() const			{ return btTransform( GetRotationQuat() ) * btVector3::Forward(); }
	btVector3 Transform::Up() const					{ return btTransform( GetRotationQuat(), btVector3( 0, 0, 0 ) ) * btVector3::Up(); }

	//================================================================================================================================
	//================================================================================================================================
	void Transform::LookAt( const btVector3& _target, const btVector3& _up ) {
		btTransform& transform = GetEcsTransform()->transform;

		btVector3 forward = _target - transform.getOrigin();
		forward.normalize();
		btVector3 left = _up.cross( forward );
		left.normalize();

		transform.setBasis( btMatrix3x3 (	left[0], _up[0], forward[0],
											left[1], _up[1], forward[1],
											left[2], _up[2], forward[2] ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetPosition(btVector3 _newPosition) {
		btTransform& transform = GetEcsTransform()->transform;
		if (transform.getOrigin() != _newPosition) {
			transform.setOrigin( _newPosition);
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
 		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetScale(btVector3 _newScale) {
		ecsScaling* ecsScale = GetEcsScale();
		if (ecsScale->scale != _newScale) {
			ecsScale->scale = _newScale;
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
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
		btTransform& transform = GetEcsTransform()->transform;
		btVector3 euler;
		transform.getRotation().getEulerZYX(euler[2], euler[1], euler[0]);
		return btDegrees3(euler);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::SetRotationQuat(const btQuaternion _rotation) {
		btTransform& transform = GetEcsTransform()->transform;
		if (transform.getRotation() != _rotation) {
			transform.setRotation( _rotation);
			m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Transform::GetModelMatrix() const {
		btTransform& transform = GetEcsTransform()->transform;
		btVector3& btScale = GetEcsScale()->scale;

		glm::vec3 position =	ToGLM(transform.getOrigin());
		glm::vec3 scale =		ToGLM(btScale);
		glm::quat rotation =	ToGLM( transform.getRotation());

		return glm::translate(glm::mat4(1.f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), scale);
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::TransformPoint(const btVector3 _point) const {
		ecsScaling* scaling = GetEcsScale();
		const btTransform transform = GetBtTransform();
		btVector3 transformedPoint = transform * ( scaling->scale * _point);
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 Transform::InverseTransformPoint(const btVector3 _point) const {
		ecsScaling* scaling = GetEcsScale();
		const btVector3 invertScale(1.f / scaling->scale[0], 1.f / scaling->scale[1], 1.f / scaling->scale[2]);
		const btTransform transform = GetBtTransform();
		btVector3 transformedPoint = invertScale * (transform.inverse()*_point);
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::TransformDirection(const btVector3 _point) const {
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = transform * ( GetScale() * _point);
		return transformedPoint;
	}

	//================================================================================================================================
	// No translation applied
	//================================================================================================================================
	btVector3 Transform::InverseTransformDirection(const btVector3 _point) const {
		ecsScaling * scaling = GetEcsScale();
		const btVector3 invertScale(1.f / scaling->scale[0], 1.f / scaling->scale[1], 1.f / scaling->scale[2]);
		const btTransform transform( GetRotationQuat() );
		btVector3 transformedPoint = invertScale * (transform.inverse()*_point);
		return transformedPoint;
	}

	//================================================================================================================================
	//================================================================================================================================
	void Transform::OnGui() {
		Component::OnGui();

		// Position
		if (ImGui::Button("##TransPos")) {
			SetPosition(btVector3(0, 0, 0));
		} ImGui::SameLine();
		float posBuffer[3] = { GetPosition().x(), GetPosition().y(), GetPosition().z() };
		if (ImGui::DragFloat3("position", posBuffer, 0.1f)) {
			SetPosition(btVector3(posBuffer[0], posBuffer[1], posBuffer[2]));
		}

		// rotation
		if (ImGui::Button("##TransRot")) {
			SetRotationQuat(btQuaternion::getIdentity());
		} ImGui::SameLine();
		const btVector3 rot = GetRotationEuler();
		float bufferAngles[3] = { rot.x(),rot.y(),rot.z() };
		if (ImGui::DragFloat3("rotation", bufferAngles, 0.1f)) {
			SetRotationEuler(btVector3(bufferAngles[0], bufferAngles[1], bufferAngles[2]));
		}

		// Scale
		if (ImGui::Button("##TransScale")) {
			SetScale(btVector3(1, 1, 1));
		} ImGui::SameLine();
		btVector3 scale = GetScale();
		if (ImGui::DragFloat3("scale", &scale[0], 0.1f)) {
			SetScale(scale);
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Transform::Load( Json & _json ) {
		btTransform& transform = GetEcsTransform()->transform;
		btVector3& scale = GetEcsScale()->scale;

		btVector3 tmpVec;
		btQuaternion tmpQuat;

		LoadVec3( _json, "position", tmpVec );
		LoadQuat( _json, "rotation", tmpQuat );
		LoadVec3( _json, "scale", scale );

		transform.setOrigin( tmpVec );
		transform.setRotation( tmpQuat );

		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Transform::Save( Json & _json ) const {
		btTransform& transform = GetEcsTransform()->transform;
		btVector3& scale = GetEcsScale()->scale;

		SaveVec3( _json, "position", transform.getOrigin() );
		SaveQuat( _json, "rotation", transform.getRotation() );
		SaveVec3( _json, "scale", scale );


		Component::Save( _json );
				
		return true;
	}


	//================================================================================================================================
	//================================================================================================================================
	ecsTranform* Transform::GetEcsTransform() const { return m_gameobject->GetEcsComponent<ecsTranform>(); }
	ecsScaling*  Transform::GetEcsScale() const		{ return m_gameobject->GetEcsComponent<ecsScaling>();  }
	
}