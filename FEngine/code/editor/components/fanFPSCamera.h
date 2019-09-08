#pragma once

#include "scene/components/fanActor.h"

namespace fan
{
	namespace scene
	{
		class Transform;
		class Camera;

		//================================================================================================================================
		//================================================================================================================================
		class FPSCamera : public Actor
		{
		public:
			virtual ~FPSCamera();

			bool			IsUnique() const	override { return true; }

			void Start() override;
			void Update(const float _delta) override;

			// Getters
			float GetSpeed() const { return m_speed; }
			float GetSpeedMultiplier() const { return m_speedMultiplier; }
			btVector2 GetXYSensitivity() const { return m_xySensitivity; }
			void SetSpeed(const float _speed) { m_speed = _speed; }
			void SetSpeedMultiplier(const float _speedMultiplier) { m_speedMultiplier = _speedMultiplier; }
			void SetXYSensitivity(const btVector2 _sensitivity) { m_xySensitivity = _sensitivity; }

			// ISerializable
			void Load(std::istream& _in) override;
			void Save(std::ostream& _out, const int _indentLevel) override;

			DECLARE_TYPE_INFO(FPSCamera);
		private:
			void Initialize() override;
			void Delete() override {};

			float m_speed;
			float m_speedMultiplier;
			btVector2 m_xySensitivity;

			scene::Transform * m_transform;
			scene::Camera * m_camera;
		};
	}
}