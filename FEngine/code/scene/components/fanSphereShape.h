#pragma once

#include "scene/components/fanComponent.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class SphereShape : public Component
	{
	public:
		void SetRadius( const float _radius );
		btSphereShape * GetBtShape();

	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
		void OnGui() override;

		DECLARE_EDITOR_COMPONENT( SphereShape )
		DECLARE_TYPE_INFO( SphereShape );
	private:

		ecsSphereShape*	GetEcsSphereShape() const;

	};
}