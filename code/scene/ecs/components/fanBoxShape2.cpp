#include "scene/ecs/components/fanBoxShape2.hpp"

#include "render/fanRenderSerializable.hpp"

namespace fan
{
	REGISTER_COMPONENT( BoxShape2, "box_shape" );

	//================================================================================================================================
	//================================================================================================================================
	BoxShape2::BoxShape2() : boxShape( btVector3( 0.5f, 0.5f, 0.5f ) ) {}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::SetInfo( ComponentInfo& _info )
	{
		_info.icon = ImGui::IconType::CUBE_SHAPE16;
		_info.onGui = &BoxShape2::OnGui;
		_info.init = &BoxShape2::Init;
		_info.load  = &BoxShape2::Load;
		_info.save  = &BoxShape2::Save;
		_info.editorPath = "/";
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::Init( ecComponent& _component )
	{
		BoxShape2& boxShape = static_cast<BoxShape2&>( _component );
		boxShape.boxShape = btBoxShape( btVector3( 0.5f, 0.5f, 0.5f ) );
		boxShape.boxShape.setUserPointer( nullptr );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::OnGui( ecComponent& _boxShape )
	{
		BoxShape2& boxShape = static_cast<BoxShape2&>( _boxShape );
		ImGui::PushItemWidth( 0.6f * ImGui::GetWindowWidth() );
		{
			btVector3 extent = boxShape.GetScaling();
			if( ImGui::DragFloat3( "scaling", &extent[0], 0.05f, 0.f ) )
			{
				boxShape.SetScaling( extent );
			}
			//RendererDebug::Get().DebugCube( m_gameobject->GetTransform().GetBtTransform(), 0.5f * extent, Color::Green );
		} ImGui::PopItemWidth();
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::Save( const ecComponent& _boxShape, Json& _json )
	{
		const BoxShape2& boxShape = static_cast<const BoxShape2&>( _boxShape );

		Serializable::SaveVec3( _json, "scaling", boxShape.GetScaling() );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::Load( ecComponent& _boxShape, const Json& _json )
	{
		BoxShape2& boxShape = static_cast<BoxShape2&>( _boxShape );

		btVector3 scaling;
		Serializable::LoadVec3( _json, "scaling", scaling );

		boxShape.SetScaling( scaling );
	}

	//================================================================================================================================
	//================================================================================================================================
	void BoxShape2::SetScaling( const btVector3 _scaling )
	{
		boxShape.setLocalScaling( _scaling );
	}

	//================================================================================================================================
	//================================================================================================================================
	btVector3 BoxShape2::GetScaling() const
	{
		return boxShape.getLocalScaling();
	}
}