#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Transform : public Component
	{
	public:
		void SetPosition(btVector3 _newPosition);
		void SetScale(btVector3 _newScale);
		void SetRotationEuler(const btVector3 _rotation);
		void SetRotationQuat(const btQuaternion _rotation);
		void LookAt( const btVector3& _target, const btVector3& _up );

		const btTransform&	GetBtTransform() const;
		btVector3			GetPosition() const;
		btVector3			GetScale() const;
		btQuaternion		GetRotationQuat() const;
		btVector3			GetRotationEuler() const;
		glm::mat4			GetModelMatrix() const;
		glm::mat4			GetNormalMatrix() const;

		btVector3 TransformPoint( const btVector3 _point ) const;
		btVector3 TransformDirection( const btVector3 _point ) const;
		btVector3 InverseTransformPoint( const btVector3 _point ) const;
		btVector3 InverseTransformDirection( const btVector3 _point ) const;

		btVector3 Left() const;
		btVector3 Forward() const;
		btVector3 Up() const;

		void			OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::TRANSFORM; }

		// ISerializable
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		DECLARE_TYPE_INFO(Transform, Component );
	
	protected:
		void OnAttach() override;
		void OnDetach() override;

	private:
		// ecs pointers are const to prevent user from accidentally breaking them
		ecsTranform * const m_transform = nullptr;
		ecsScaling *  const m_scale = nullptr;
	};
}