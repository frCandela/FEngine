#pragma once

#include "scene/components/fanColliderShape.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class BoxShape : public ColliderShape {
	public:
		void SetExtent( const btVector3 _extent );
		btVector3 GetExtent() const;

		btBoxShape *		GetBoxShape();
		btCollisionShape *	GetCollisionShape() override;

		ImGui::IconType GetIcon() const override { return ImGui::IconType::BOX_SHAPE; };
		void OnGui() override;
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

		DECLARE_TYPE_INFO( BoxShape, Component );
	private:

		btBoxShape* const m_boxShape = nullptr;
	};
}