#pragma once

#include "scene/components/fanColliderShape.h"

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
		ImGui::IconType GetIcon() const override  { return ImGui::IconType::SPHERE_SHAPE; };

	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;


		DECLARE_EDITOR_COMPONENT( SphereShape )
		DECLARE_TYPE_INFO( SphereShape, Component );
	private:		
		ecsSphereShape*	GetEcsSphereShape() const;

	};
}