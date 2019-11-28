#include "fanGlobalIncludes.h"
#include "scene/components/ui/fanUITransform.h"

#include "scene/fanGameobject.h"
#include "scene/components/fanTransform.h"
#include "core/input/fanInput.h"

namespace fan
{
	REGISTER_TYPE_INFO( UITransform, TypeInfo::Flags::EDITOR_COMPONENT, "ui/" )

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::OnAttach()
	{
		Component::OnAttach();
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::OnDetach()
	{		
		Component::OnDetach();
	}

	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	 UITransform::GetPixelPosition() const
	{

		btVector2 pixelPos = 0.5f *( m_gameobject->GetTransform()->GetPosition() + btVector3::One() ) * Input::Get().WindowSizeF();
		return glm::ivec2( (int) pixelPos.x(),(int)pixelPos.y() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::SetPixelPosition( const glm::ivec2 _position )
	{
		Transform * transform = m_gameobject->GetTransform();
		const btVector2 newPosition = 2.f * btVector2((float)_position.x,(float)_position.y) / Input::Get().WindowSizeF() - btVector3::One();
		transform->SetPosition( btVector3( newPosition.x(), newPosition.y(), transform->GetPosition().z() ) );		
	}
	
	//================================================================================================================================
	//================================================================================================================================
	glm::ivec2	 UITransform::GetPixelSize() const
	{ 
		btVector2 pixelSize = m_gameobject->GetTransform()->GetScale() * Input::Get().WindowSizeF();
		return glm::ivec2( (int)pixelSize[0],(int)pixelSize[1] );
	}
	
	//================================================================================================================================
	//================================================================================================================================
	void UITransform::SetPixelSize( const glm::ivec2 _size )
	{
		Transform * transform = m_gameobject->GetTransform();
		btVector2 screenSize = btVector2((float)_size.x,(float)_size.y) / Input::Get().WindowSizeF();
		transform->SetScale( btVector3( screenSize.x(), screenSize.y(), transform->GetScale().z() ) );
	}

	//================================================================================================================================
	//================================================================================================================================
	void UITransform::OnGui()
	{
		 const btVector2 windowSize = Input::Get().WindowSizeF();

		Component::OnGui();

		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() - 16 );
		{
			// pixel pos reset		
			if ( ImGui::Button( "##reset ui pos" ) ) { SetPixelPosition(Input::Get().WindowSize() / 2);	} 
			ImGui::SameLine();

			// pixel pos
			glm::ivec2 pixelPos = GetPixelPosition();
			if ( ImGui::DragInt2( "pixel position", &pixelPos[0] ) ){ SetPixelPosition(pixelPos); }

			// pixel size reset		
			if ( ImGui::Button( "##reset ui size" ) ) {	SetPixelSize( {10,10} ); } ImGui::SameLine();

			// pixel size
			glm::ivec2 pixelSize = GetPixelSize();
			if ( ImGui::DragInt2( "pixel size", &pixelSize.x, 1.f, 0, 10000 ) )	{ SetPixelSize( pixelSize ); }

		} ImGui::PopItemWidth();


	}

	//================================================================================================================================
	//================================================================================================================================
	bool UITransform::Load( const Json & _json )
	{
		Component::Load(_json);

		return true;
	}

	//==========================z======================================================================================================
	//================================================================================================================================
	bool UITransform::Save( Json & _json ) const
	{
		Component::Save( _json );
		return true;
	}
}