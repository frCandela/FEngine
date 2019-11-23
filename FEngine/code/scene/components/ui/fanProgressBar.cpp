#include "fanGlobalIncludes.h"
#include "scene/components/ui/fanProgressBar.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "scene/components/ui/fanUIMeshRenderer.h"

namespace fan
{
	REGISTER_TYPE_INFO( ProgressBar, TypeInfo::Flags::EDITOR_COMPONENT )


	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnAttach()
	{
		Component::OnAttach();

		UIMeshRenderer * image = m_gameobject->GetComponent<UIMeshRenderer>();
		if ( image != nullptr )
		{
			m_image = image;
			m_maxWidth = image->GetPixelSize().x;
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
			if ( *m_image != nullptr )
			{
				const glm::ivec2 pos = m_image->GetPixelPosition();
				const glm::ivec2 size = m_image->GetPixelSize();

				m_image->SetPixelSize( glm::ivec2( m_progress * m_maxWidth, size.y ) );			
			}
		}		
	}

	//================================================================================================================================
	//================================================================================================================================
	void ProgressBar::OnGui()
	{
		ImGui::FanComponent("image", &m_image);

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
		SaveComponentPtr(_json, "image", m_image );
		SaveInt( _json, "max_width", m_maxWidth );
		return true;
	}
	 
	//================================================================================================================================
	//================================================================================================================================
	bool ProgressBar::Load( const Json & _json )
	{
		LoadComponentPtr(_json, "image", m_image );
		LoadInt( _json, "max_width", m_maxWidth );
		return true;
	}
}