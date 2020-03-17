#pragma once

#include "scene/fanScenePrecompiled.hpp"

#include "scene/fanComponentPtr.hpp"

namespace fan
{
	//class UITransform;

	//================================================================================================================================
	//================================================================================================================================
// 	class ProgressBar : public Component
// 	{
// 	public:
// 
// 		void OnGui() override;
// 		ImGui::IconType GetIcon() const override { return ImGui::IconType::UI_PROGRESS_BAR16; }
// 
// 		void	SetProgress( const float _progress );
// 		float	GetProgress() const { return m_progress; }
// 
// 		DECLARE_TYPE_INFO( ProgressBar, Component );
// 	protected:
// 		void OnAttach() override;
// 		void OnDetach() override;
// 		bool Load( const Json& _json ) override;
// 		bool Save( Json& _json ) const override;
// 
// 	private:
// 		//ComponentPtr<UITransform> m_targetUiTransform;
// 		float	m_progress = -1.f;
// 		int		m_maxWidth = 100;
// 	};
}