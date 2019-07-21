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
		virtual ~FPSCamera();

		bool			IsUnique() const	override { return true; }
		const char *	GetName() const		override { return s_name; }
		uint32_t		GetType()	  const	override { return s_type; }
		Component *		NewInstance(Gameobject * _gameobject) const override { return new FPSCamera(_gameobject); }

		void Start() override;
		void Update( const float _delta ) override;

		// Getters
		float GetSpeed() const				{ return m_speed; }
		float GetSpeedMultiplier() const	{ return m_speedMultiplier; }
		btVector2 GetXYSensitivity() const	{ return m_xySensitivity; }
		void SetSpeed( const float _speed)						{ m_speed = _speed; }
		void SetSpeedMultiplier( const float _speedMultiplier)  { m_speedMultiplier= _speedMultiplier; }
		void SetXYSensitivity( const btVector2 _sensitivity)		{ m_xySensitivity= _sensitivity; }

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		const static char * s_name;
		static const uint32_t s_type;
	private:
		float m_speed;
		float m_speedMultiplier;
		btVector2 m_xySensitivity;

		scene::Transform * m_transform;
		scene::Camera * m_camera;
	};
}