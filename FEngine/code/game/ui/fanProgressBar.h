#pragma once

#include "scene/actors/fanActor.h"
#include "scene/fanComponentPtr.h"

namespace fan
{
	class UIMeshRenderer;

	//================================================================================================================================
	//================================================================================================================================
	class ProgressBar : public Component
	{
	public:

		void OnGui() override;
		ImGui::IconType GetIcon() const override { return ImGui::IconType::PROGRESS_BAR; }

		DECLARE_TYPE_INFO( ProgressBar, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( Json & _json ) override;
		bool Save( Json & _json ) const override;
	private:

		ComponentPtr<UIMeshRenderer> m_outsideImage;
		ComponentPtr<UIMeshRenderer> m_insideImage;
	};
}