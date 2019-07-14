#pragma once

#include "scene/components/fanActor.h"

namespace scene
{
	class Transform;
	class Camera;

	class FPSCamera : public Actor
	{
	public:
		FPSCamera(Gameobject * _gameobject);

		bool IsUnique() const override { return true; }
		std::string GetName() const override { return "fps camera"; }

		void Start() override;
		void Update( const float _delta ) override;
		void Stop() override;

		// Getters
		float GetSpeed() const				{ return m_speed; }
		float GetSpeedMultiplier() const	{ return m_speedMultiplier; }
		btVector2 GetXYSensitivity() const	{ return m_xySensitivity; }
		void SetSpeed( const float _speed)						{ m_speed = _speed; }
		void SetSpeedMultiplier( const float _speedMultiplier)  { m_speedMultiplier= _speedMultiplier; }
		void SetXYSensitivity( const btVector2 _sensitivity)		{ m_xySensitivity= _sensitivity; }

	private:
		float m_speed;
		float m_speedMultiplier;
		btVector2 m_xySensitivity;

		scene::Transform & m_transform;
		scene::Camera & m_camera;
	};
}