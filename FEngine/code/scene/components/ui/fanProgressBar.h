#pragma once

#include "scene/components/fanComponent.h"
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

		void	SetProgress( const float _progress );
		float	GetProgress() const { return m_progress; }

		UIMeshRenderer* GetRenderer() const { return *m_image; }

		DECLARE_TYPE_INFO( ProgressBar, Component );
	protected:
		void OnAttach() override;
		void OnDetach() override;
		bool Load( const Json & _json ) override;
		bool Save( Json & _json ) const override;

	private:
		ComponentPtr<UIMeshRenderer> m_image;
		float	m_progress = 1.f;
		int		m_maxWidth = 100;
	};
}