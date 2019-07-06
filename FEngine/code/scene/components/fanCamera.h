#pragma once

#include "scene/components/fanComponent.h"
#include "util/shapes/fanRay.h"

namespace util { class ray; }
namespace scene
{
	class Camera : public Component
	{
	public:

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "camera"; }

		glm::mat4 GetView() const;
		glm::mat4 GetProjection() const;
		util::Ray ScreenPosToRay(glm::vec2 screenSpacePosition); // Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
	
		float GetFov() const			{ return m_fov; };
		float GetNearDistance() const	{ return m_nearDistance; };
		float GetFarDistance() const	{ return m_farDistance; };

		void SetFov(float _fov);
		void SetNearDistance(float _nearDistance);
		void SetFarDistance(float _farDistance);
		void SetAspectRatio(float _aspectRatio);

	private:
		float m_fov = 90.f;
		float m_aspectRatio = 1.f;
		float m_nearDistance = 0.1f;
		float m_farDistance = 1000.f;
	};
}