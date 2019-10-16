#pragma once

#include "scene/components/fanComponent.h"
#include "core/fanSignal.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class ColliderShape : public Component {
	public:
		bool IsCollider() const override { return true; }

		virtual btCollisionShape * GetCollisionShape() = 0;

		DECLARE_ABSTRACT_TYPE_INFO( ColliderShape, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnGui() override;
	private:


	};
}