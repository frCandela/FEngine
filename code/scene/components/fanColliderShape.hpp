#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/components/fanComponent.hpp"
#include "core/fanSignal.hpp"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class ColliderShape : public Component
	{
	public:
		bool IsCollider() const override { return true; }

		virtual btCollisionShape* GetCollisionShape() = 0;

		DECLARE_ABSTRACT_TYPE_INFO( ColliderShape, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;
		void OnGui() override;
	private:


	};
}