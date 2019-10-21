#include "fanGlobalIncludes.h"

#include "scene/components/fanCamera.h"
#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	REGISTER_EDITOR_COMPONENT(Camera);
	REGISTER_TYPE_INFO(Camera)

	//================================================================================================================================
	//================================================================================================================================
	void Camera::OnAttach() {
		m_fov = 110.f;
		m_aspectRatio = 1.f;
		m_nearDistance = 0.01f;
		m_farDistance = 1000.f;
		m_orthoSize = 10.f;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetView() const
	{
		Transform* transform = m_gameobject->GetComponent<Transform>();
		glm::mat4 view = glm::lookAt(
			ToGLM(transform->GetPosition()),
			ToGLM(transform->GetPosition() + transform->Forward()),
			ToGLM(transform->Up()));
		return view;
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::mat4 Camera::GetProjection() const
	{
		glm::mat4 proj = glm::mat4(1);
		if ( m_type == Type::ORTHOGONAL ) {
			proj = glm::ortho(-m_orthoSize * m_aspectRatio, m_orthoSize * m_aspectRatio,-m_orthoSize, m_orthoSize,m_nearDistance, m_farDistance);
		} 
		else if( m_type == Type::PERSPECTIVE )  {
			const float fov = glm::radians( m_fov );
			proj = glm::perspective( fov, m_aspectRatio, m_nearDistance, m_farDistance );
		}
		return proj;
	}

	//================================================================================================================================
	//================================================================================================================================
	Ray Camera::ScreenPosToRay(const btVector2& _position)
	{
		assert(_position.x() >= -1.f  && _position.x() <= 1.f);
		assert(_position.y() >= -1.f && _position.y() <= 1.f);

		if( m_type == Type::PERSPECTIVE ) {
			Transform* transform = m_gameobject->GetTransform();

			const btVector3	pos = transform->GetPosition();
			const btVector3 upVec = transform->Up();
			const btVector3 left = transform->Left();
			const btVector3 forward = transform->Forward();

			btVector3 nearMiddle = pos + m_nearDistance * forward;

			float nearHeight = m_nearDistance * tan( glm::radians( m_fov / 2 ) );
			float nearWidth = m_aspectRatio * nearHeight;

			Ray ray;
			ray.origin = nearMiddle - _position.x() * nearWidth * left - _position.y() * nearHeight * upVec;
			ray.direction = ( 100.f * ( ray.origin - pos ) ).normalized();

			return ray;
		} else { // ORTHOGONAL
			Ray ray;

			Transform * transform = m_gameobject->GetTransform();

			ray.origin = transform->GetPosition();
			ray.origin -= m_aspectRatio * m_orthoSize * transform->Left()  * _position[0];
			ray.origin -= m_orthoSize * transform->Up()  * _position[1];
			
			ray.direction = transform->Forward();

			return ray;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector2 Camera::WorldPosToScreen(const btVector3& worldPosition) {
		const glm::vec4 pos(worldPosition[0], worldPosition[1], worldPosition[2], 1.f);
		glm::vec4  proj = GetProjection() * GetView() * pos;
		proj /= proj.z;
		return btVector2(proj.x, proj.y);
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::SetFov(float _fov) {
		m_fov = _fov;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	};
	void Camera::SetOrthoSize( float _orthoSize ) {
		m_orthoSize = _orthoSize;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	};
	void Camera::SetNearDistance(float _nearDistance) {
		m_nearDistance = _nearDistance;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	};
	void Camera::SetFarDistance(float _farDistance) {
		m_farDistance = _farDistance;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	};
	void Camera::SetAspectRatio(float _aspectRatio) {
		m_aspectRatio = _aspectRatio;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	};
	void Camera::SetProjectionType( const Type _type ) {
		m_type = _type;
		m_gameobject->AddFlag( Gameobject::Flag::OUTDATED_TRANSFORM );
	}

	//================================================================================================================================
	//================================================================================================================================
	void Camera::OnGui() {
		Component::OnGui();

		int item = static_cast<int>(m_type);
		if ( ImGui::Combo( "type", &item, "perspective\0orthogonal\0" ) ) {
			SetProjectionType( Type( item ) );
		}

		ImGui::Indent();
		if ( m_type == Type::PERSPECTIVE ) {
			// fov
			if (ImGui::Button("##fov")) {
				SetFov(110.f);
			}
			ImGui::SameLine();
			float fov = GetFov();
			if (ImGui::DragFloat("fov", &fov, 1.f, 1.f, 179.f)) {
				SetFov(fov);
			}
		} else if ( m_type == Type::ORTHOGONAL ) {
			// fov
			if ( ImGui::Button( "##size" ) ) {
				m_orthoSize = 10.f;
			}
			ImGui::SameLine();
			if ( ImGui::DragFloat( "size", &m_orthoSize, 1.f, 0.f, 100.f ) ) {
				SetOrthoSize(m_orthoSize);
			}
		}

		// nearDistance
		if ( ImGui::Button( "##nearDistance" ) ) {
			SetNearDistance( 0.01f );
		}
		ImGui::SameLine();
		float near = GetNearDistance();
		if ( ImGui::DragFloat( "near distance", &near, 0.001f, 0.01f, 10.f ) ) {
			SetNearDistance( near );
		}

		// far distance
		if ( ImGui::Button( "##fardistance" ) ) {
			SetFarDistance( 1000.f );
		}
		ImGui::SameLine();
		float far = GetFarDistance();
		if ( ImGui::DragFloat( "far distance", &far, 10.f, 0.05f, 10000.f ) ) {
			SetFarDistance( far );
		}

		ImGui::Unindent();
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Camera::Load( Json & _json ) {
		int tmp;
		LoadInt  ( _json, "camera_type", tmp ); m_type = Type(tmp);
		LoadFloat( _json, "orthoSize", m_orthoSize );
		LoadFloat( _json, "fov", m_fov );
		LoadFloat( _json, "nearDistance", m_nearDistance );
		LoadFloat( _json, "farDistance", m_farDistance );
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Camera::Save( Json & _json ) const {

		SaveInt  ( _json, "camera_type", m_type );
		SaveFloat( _json, "orthoSize", m_orthoSize );
		SaveFloat( _json, "fov", m_fov );
		SaveFloat( _json, "nearDistance", m_nearDistance );
		SaveFloat( _json, "farDistance", m_farDistance );
		Component::Save( _json );
		
		return true;
	}
}