#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanColliderShape.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SphereShape : public ColliderShape
	{
	public:
		void SetRadius( const float _radius );
		float GetRadius() const;

		btSphereShape *		GetSphereShape();
		btCollisionShape *	GetCollisionShape() override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override  { return ImGui::IconType::SPHERE_SHAPE16; };

	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		DECLARE_TYPE_INFO( SphereShape, Component );
	private:		
		btSphereShape* const	m_sphereShape = nullptr;
	};
}