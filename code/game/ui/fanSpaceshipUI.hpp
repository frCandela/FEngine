#pragma once

#include "game/fanGamePrecompiled.hpp"

#include "scene/actors/fanActor.hpp"
#include "scene/fanComponentPtr.hpp"

namespace fan
{
	class ProgressBar;
	class UIRenderer;

	//================================================================================================================================
	//================================================================================================================================
	class SpaceShipUI : public Actor
	{
	public:

		void Start() override;
		void Stop() override {}
		void Update( const float _delta ) override;
		void LateUpdate( const float _delta ) override;

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::SPACE_SHIP16; }

		DECLARE_TYPE_INFO( SpaceShipUI, Actor );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json& _json ) override;
		bool Save( Json& _json ) const override;

	private:
		GameobjectPtr			  m_spaceShip;
		ComponentPtr<ProgressBar> m_healthProgress;
		ComponentPtr<ProgressBar> m_energyProgress;
		ComponentPtr<ProgressBar> m_signalProgress;
		ComponentPtr<UIRenderer> m_signalRenderer;


		Color GetSignalColor( const float _ratio );
	};
}