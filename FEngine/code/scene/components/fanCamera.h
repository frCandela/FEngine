#pragma once

#include "scene/components/fanComponent.h"
#include "core/math/shapes/fanRay.h"

namespace shape { class ray; }
namespace scene
{
	//================================================================================================================================
	//================================================================================================================================
	class Camera : public Component
	{
	public:
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

		// ISerializable
		void Load(std::istream& _in) override;
		void Save(std::ostream& _out) override;

		DECLARE_TYPE_INFO(Camera);
	private:
		void Initialize() override;
		void Delete() override {};

		float m_fov;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;
	};
}