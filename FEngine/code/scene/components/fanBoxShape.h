#pragma once

#include "scene/components/fanComponent.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class BoxShape : public Component {
	public:
		void SetHalfExtent( const btVector3 _halfExtent );
		btBoxShape * GetBtShape();
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnGui() override;

		DECLARE_EDITOR_COMPONENT( BoxShape )
		DECLARE_TYPE_INFO( BoxShape );
	private:

		ecsBoxShape*	GetEcsBoxShape() const;
	};
}