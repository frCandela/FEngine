#pragma once

#include "scene/components/fanComponent.h"
#include "core/math/shapes/fanRay.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Camera : public Component
	{
	public:
		enum Type{ PERSPECTIVE, ORTHOGONAL };

		glm::mat4 GetView() const;
		glm::mat4 GetProjection() const;
		Ray ScreenPosToRay(const btVector2& _screenSpacePosition); // Returns a ray going from camera through a screen point ( with screenSpacePosition between {-1.f,-1.f} and {1.f,1.f} ).
		btVector2 WorldPosToScreen(const btVector3& worldPosition);

		float GetFov() const { return m_fov; };
		float GetOrthoSize() const { return m_orthoSize; };
		float GetNearDistance() const { return m_nearDistance; };
		float GetFarDistance() const { return m_farDistance; };
		Type  GetProjectionType() const { return m_type; }

		void SetFov(float _fov);
		void SetOrthoSize( float _orthoSize );
		void SetNearDistance(float _nearDistance);
		void SetFarDistance(float _farDistance);
		void SetAspectRatio(float _aspectRatio);
		void SetProjectionType( const Type _type );

		void OnGui() override;

		DECLARE_EDITOR_COMPONENT(Camera)
		DECLARE_TYPE_INFO(Camera, Component );
	protected:
		void OnAttach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		float m_fov;
		float m_orthoSize;
		float m_aspectRatio;
		float m_nearDistance;
		float m_farDistance;

		Type m_type = Type::PERSPECTIVE;
	};
}