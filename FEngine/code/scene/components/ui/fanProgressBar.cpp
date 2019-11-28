#include "fanGlobalIncludes.h"
#include "scene/components/ui/fanProgressBar.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/ui/fanUITransform.h"

namespace fan
{
	REGISTER_TYPE_INFO( ProgressBar, TypeInfo::Flags::EDITOR_COMPONENT, "ui/" )


	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnAttach()
	{
		Component::OnAttach();
			   
		if ( *m_targetUiTransform != nullptr )
		{
			m_maxWidth = m_targetUiTransform->GetPixelSize().x;
		}
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnDetach()		
	{
		Component::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void  ProgressBar::SetProgress( const float _progress )
	{
		if ( _progress != m_progress )
		{
			m_progress = _progress;
			if ( *m_targetUiTransform != nullptr )
			{
				const glm::ivec2 pos = m_targetUiTransform->GetPixelPosition();
				const glm::ivec2 size = m_targetUiTransform->GetPixelSize();

				m_targetUiTransform->SetPixelSize( glm::ivec2( m_progress * m_maxWidth, size.y ) );			
			}
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnGui()
	{
		ImGui::FanComponent("target ui transform", &m_targetUiTransform);

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			float progress = m_progress;
			if ( ImGui::DragFloat( "progress", &progress, 0.01f, 0.f, 1.f ) ){ SetProgress(progress); }
			ImGui::DragInt("max width", &m_maxWidth, 1.f, 0 );
		} ImGui::PopItemWidth();

	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool ProgressBar::Save( Json & _json ) const
	{
		Component::Save( _json );
		SaveComponentPtr(_json, "target_ui_transform", m_targetUiTransform );
		SaveInt( _json, "max_width", m_maxWidth );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool ProgressBar::Load( const Json & _json )
	{
		LoadComponentPtr(_json, "target_ui_transform", m_targetUiTransform );
		LoadInt( _json, "max_width", m_maxWidth );
		return true;
	}
}