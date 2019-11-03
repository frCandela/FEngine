#pragma once

#include "scene/components/fanComponent.h"
#include "core/fanSignal.h"

namespace fan
{
	//================================================================================================================================
	//================================================================================================================================
	class Actor : public Component
	{
	public:
		static Signal< Actor * > onActorAttach;
		static Signal< Actor * > onActorDetach;

		virtual void Start() = 0;
		virtual void Update(const float _delta) = 0;
		virtual void LateUpdate( const float _delta ) = 0;

		bool IsActor() const override { return true; }
		void OnGui() override;
		bool IsEnabled() const { return m_isEnabled; }
		void SetEnabled(const bool _enabled);

		DECLARE_ABSTRACT_TYPE_INFO( Actor, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

		virtual void OnEnable() {};
		virtual void OnDisable(){};
	private:
		bool m_isEnabled = true;

	};
}