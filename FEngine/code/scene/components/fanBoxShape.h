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
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnGui() override;

		DECLARE_EDITOR_COMPONENT( BoxShape )
		DECLARE_TYPE_INFO( BoxShape, Component );
	private:

		ecsBoxShape*	GetEcsBoxShape() const;
	};
}