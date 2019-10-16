#pragma once

#include "scene/components/fanActor.h"

namespace fan {
	//================================================================================================================================
	//================================================================================================================================
	class Planet : public Actor {
	public:
		void Start() override;
		void Update(const float _delta) override;

		void OnGui() override;

		void SetSpeed( const float _speed );
		void SetRadius( const float _radius );
		void SetPhase( const float _phase );

		DECLARE_EDITOR_COMPONENT(Planet)
		DECLARE_TYPE_INFO(Planet, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:

		ecsPlanet* Planet::GetEcsPlanet() const;
	};
}