#include "fanGlobalIncludes.h"

#include "scene/components/fanCamera.h"
#include "scene/fanEntity.h"
#include "scene/components/fanTransform.h"

namespace fan
{
	namespace scene
	{
		REGISTER_TYPE_INFO(Camera)

			//================================================================================================================================
			//================================================================================================================================
			void Camera::Initialize() {
			m_fov = 110.f;
			m_aspectRatio = 1.f;
			m_nearDistance = 0.01f;
			m_farDistance = 1000.f;
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
			const float fov = glm::radians(m_fov);
			glm::mat4 proj = glm::perspective(fov, m_aspectRatio, m_nearDistance, m_farDistance);
			return proj;
		}

		//================================================================================================================================
		//================================================================================================================================
		shape::Ray Camera::ScreenPosToRay(const btVector2& _position)
		{
			assert(_position.x() >= -1.f  && _position.x() <= 1.f);
			assert(_position.y() >= -1.f && _position.y() <= 1.f);

			Transform* transform = GetEntity()->GetComponent<Transform>();

			const btVector3	 pos = transform->GetPosition();
			const btVector3 upVec = transform->Up();
			const btVector3 right = transform->Right();
			const btVector3 forward = transform->Forward();

			btVector3 nearMiddle = pos + m_nearDistance * forward;

			float nearHeight = m_nearDistance * tan(glm::radians(m_fov / 2));
			float nearWidth = m_aspectRatio * nearHeight;

			shape::Ray ray;
			ray.origin = nearMiddle + _position.x() * nearWidth * right - _position.y() * nearHeight * upVec;
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
			SetModified(true);
		};
		void Camera::SetNearDistance(float _nearDistance) {
			m_nearDistance = _nearDistance;
			SetModified(true);
		};
		void Camera::SetFarDistance(float _farDistance) {
			m_farDistance = _farDistance;
			SetModified(true);
		};
		void Camera::SetAspectRatio(float _aspectRatio) {
			m_aspectRatio = _aspectRatio;
			SetModified(true);
		};

		//================================================================================================================================
		//================================================================================================================================
		bool Camera::Load(std::istream& _in) {
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
		bool Camera::Save(std::ostream& _out, const int _indentLevel) const  {
			const std::string indentation = GetIndentation(_indentLevel);
			_out << indentation << "fov:          " << m_fov << std::endl;
			_out << indentation << "nearDistance: " << m_nearDistance << std::endl;
			_out << indentation << "farDistance: " << m_farDistance << std::endl;
			return true;
		}
	}
}