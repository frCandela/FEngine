#include "fanGlobalIncludes.h"

#include "scene/components/fanCamera.h"
#include "scene/fanEntity.h"
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
		Transform* transform = GetEntity()->GetComponent<Transform>();
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

		Transform* transform = GetEntity()->GetComponent<Transform>();

		const btVector3	pos = transform->GetPosition();
		const btVector3 upVec = transform->Up();
		const btVector3 left = transform->Left();
		const btVector3 forward = transform->Forward();

		btVector3 nearMiddle = pos + m_nearDistance * forward;

		float nearHeight = m_nearDistance * tan(glm::radians(m_fov / 2));
		float nearWidth = m_aspectRatio * nearHeight;

		Ray ray;
		ray.origin = nearMiddle - _position.x() * nearWidth * left - _position.y() * nearHeight * upVec;
		ray.direction = (100.f * (ray.origin - pos)).normalized();

		return ray;
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
		MarkModified();
	};
	void Camera::SetOrthoSize( float _orthoSize ) {
		m_orthoSize = _orthoSize;
		MarkModified();
	};
	void Camera::SetNearDistance(float _nearDistance) {
		m_nearDistance = _nearDistance;
		MarkModified();
	};
	void Camera::SetFarDistance(float _farDistance) {
		m_farDistance = _farDistance;
		MarkModified();
	};
	void Camera::SetAspectRatio(float _aspectRatio) {
		m_aspectRatio = _aspectRatio;
		MarkModified();
	};
	void Camera::SetProjectionType( const Type _type ) {
		m_type = _type;
		MarkModified();
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
	bool Camera::Load(std::istream& _in) {
		if ( !ReadSegmentHeader( _in, "type:" ) ) { return false; }
		int type;
		if ( !ReadInteger( _in, type ) ) { return false; }
		m_type = Type( type );

		if ( !ReadSegmentHeader( _in, "orthoSize:" ) ) { return false; }
		if ( !ReadFloat( _in, m_orthoSize ) ) { return false; }

		if (!ReadSegmentHeader(_in, "fov:")) { return false; }
		if (!ReadFloat(_in, m_fov)) { return false; }

		if (!ReadSegmentHeader(_in, "nearDistance:")) { return false; }
		if (!ReadFloat(_in, m_nearDistance)) { return false; }

		if (!ReadSegmentHeader(_in, "farDistance:")) { return false; }
		if (!ReadFloat(_in, m_farDistance)) { return false; }
		return true;
	}

	//================================================================================================================================
	//================================================================================================================================
	bool Camera::Save(std::ostream& _out, const int _indentLevel) const {
		const std::string indentation = GetIndentation(_indentLevel);
		_out << indentation << "type:         " << m_type << std::endl;
		_out << indentation << "orthoSize:    " << m_orthoSize << std::endl;
		_out << indentation << "fov:          " << m_fov << std::endl;
		_out << indentation << "nearDistance: " << m_nearDistance << std::endl;
		_out << indentation << "farDistance:  " << m_farDistance << std::endl;
		return true;
	}
}