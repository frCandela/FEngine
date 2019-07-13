#pragma once

#include "scene/components/fanComponent.h"
#include "util/shapes/fanRay.h"

namespace shape { class ray; }
namespace scene
{
	class Camera : public Component
	{
	public:
		Camera(Gameobject * _gameobject);

		glm::mat4 GetView() const;
		glm::mat4 GetProjection() const;
		shape::Ray ScreenPosToRay(btVector2 _screenSpacePosition); // Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
	
		float GetFov() const			{ return m_fov; };
		float GetNearDistance() const	{ return m_nearDistance; };
		float GetFarDistance() const	{ return m_farDistance; };

		void SetFov(float _fov);
		void SetNearDistance(float _nearDistance);
		void SetFarDistance(float _farDistance);
		void SetAspectRatio(float _aspectRatio);

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "camera"; }

	private:
		float m_fov;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;
	};
}