#pragma once

#include "scene/components/fanComponent.h"
#include "util/shapes/fanRay.h"

namespace shape { class ray; }
namespace scene
{
	class Camera : public Component
	{
	public:
		Camera();
		Camera::Camera(Gameobject * _gameobject);

		glm::mat4 GetView() const;
		glm::mat4 GetProjection() const;
		shape::Ray ScreenPosToRay( const btVector2& _screenSpacePosition); // Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
		btVector2 WorldPosToScreen(const btVector3& worldPosition);

		float GetFov() const			{ return m_fov; };
		float GetNearDistance() const	{ return m_nearDistance; };
		float GetFarDistance() const	{ return m_farDistance; };

		void SetFov(float _fov);
		void SetNearDistance(float _nearDistance);
		void SetFarDistance(float _farDistance);
		void SetAspectRatio(float _aspectRatio);

		bool			IsUnique()		const override { return true; }
		const char *	GetName()		const override { return s_name; }
		uint32_t		GetType()		const override { return s_type; }
		Component *		NewInstance(Gameobject * _gameobject) const override { return new Camera(_gameobject); }

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		const static char * s_name;
		static const uint32_t s_type;
	private:
		float m_fov;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;
	};
}