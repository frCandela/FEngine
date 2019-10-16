#pragma once

#include "scene/components/fanActor.h"

namespace fan
{
	class Transform;
	class Camera;

	//================================================================================================================================
	//================================================================================================================================
	class FPSCamera : public Actor
	{
	public:
		virtual ~FPSCamera();

		void Start() override;
		void Update(const float _delta) override;

		// Getters
		float GetSpeed() const				{ return m_speed; }
		float GetSpeedMultiplier() const	{ return m_speedMultiplier; }
		btVector2 GetXYSensitivity() const	{ return m_xySensitivity; }
		void SetSpeed(const float _speed)	{ m_speed = _speed; }
		void SetSpeedMultiplier(const float _speedMultiplier)	{ m_speedMultiplier = _speedMultiplier; }
		void SetXYSensitivity(const btVector2 _sensitivity)		{ m_xySensitivity = _sensitivity; }

		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(FPSCamera);
		DECLARE_TYPE_INFO(FPSCamera, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		void OnDisable() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_speed;
		float m_speedMultiplier;
		btVector2 m_xySensitivity;

		Transform * m_transform;
		Camera * m_camera;
	};
}